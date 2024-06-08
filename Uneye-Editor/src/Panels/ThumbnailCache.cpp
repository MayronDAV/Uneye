#include "ThumbnailCache.h"

#include "Uneye/Asset/TextureImporter.h"

#include <chrono>

#include <iostream>
#include <string>
#include <fstream>

#include <thread>
#include <stb/include/stb_image.h>



namespace Uneye 
{
	static std::unordered_set<std::string_view> s_TextureExtensions = {
		".png", ".jpg", ".jpeg"
	};


	ThumbnailCache::ThumbnailCache(Ref<Project> project)
		: m_Project(project), m_ThumbnailCachePath(project->GetCacheDirectory()), m_StopThread(false), m_Pool(4), m_WorkerThread(&ThumbnailCache::ProcessQueue, this)
	{
	}

	ThumbnailCache::~ThumbnailCache()
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_StopThread = true;
		}

		m_Condition.notify_all();
		if (m_WorkerThread.joinable())
			m_WorkerThread.join();

		m_Pool.stopThreads();
	}

	Ref<Texture2D> ThumbnailCache::GetOrCreateThumbnail(const std::filesystem::path& p_assetPath)
	{
		auto absolutePath = m_Project->GetAssetAbsolutePath(p_assetPath);
		std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(absolutePath);
		uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();

		if (auto texture = GetCachedImage(absolutePath, timestamp); texture != nullptr)
			return texture;

		auto it = s_TextureExtensions.find(absolutePath.extension().generic_string());
		if (it == s_TextureExtensions.end())
			return nullptr;

		AddToQueue(absolutePath, timestamp);
		return nullptr;
	}

	void ThumbnailCache::OnUpdate()
	{
		std::lock_guard<std::mutex> lock(m_ProcessedMutex);
		while (!m_ProcessedQueue.empty())
		{
			auto& [path, texInfo, timestamp] = m_ProcessedQueue.front();

			if (auto texture = GetCachedImage(path, timestamp); texture != nullptr)
			{
				stbi_image_free(texInfo.Data);
				m_ProcessedQueue.pop();
				continue;
			}

			Ref<Texture2D> texture = Texture2D::Create(texInfo.Spec, { texInfo.Data, (uint64_t)(texInfo.Spec.Width * texInfo.Spec.Height * texInfo.Channels) });
			if (texture)
			{
				texture->ChangeSize(texInfo.newWidth, texInfo.newHeight);

				auto& cachedImage = m_CachedImages[path];
				cachedImage.Timestamp = timestamp;
				cachedImage.Image = texture;
			}
			else
			{
				UNEYE_CORE_ERROR("Failed to create texture {}", path.string());
			}

			stbi_image_free(texInfo.Data);
			m_QueuedItems.erase(path);
			m_ProcessedQueue.pop();
		}
	}


	void ThumbnailCache::AddToQueue(const std::filesystem::path& path, uint64_t timestamp)
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			if (m_QueuedItems.find(path) == m_QueuedItems.end())
			{
				m_Queue.push({ path, timestamp });
				m_QueuedItems.insert(path);
			}
		}
		m_Condition.notify_one();
	}


	Ref<Texture2D> ThumbnailCache::GetCachedImage(const std::filesystem::path& path, uint64_t timestamp)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		if (m_CachedImages.find(path) != m_CachedImages.end())
		{
			auto& cachedImage = m_CachedImages.at(path);
			if (cachedImage.Timestamp == timestamp)
				return m_CachedImages[path].Image;
		}
		return nullptr;
	}

	void ThumbnailCache::ProcessQueue()
	{
		while (true)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_Condition.wait(lock, [this] { return !m_Queue.empty() || m_StopThread; });

			if (m_StopThread && m_Queue.empty())
				break;

			if (!m_Queue.empty())
			{
				auto [path, timestamp] = m_Queue.front();
				m_Queue.pop();

				lock.unlock();

				if (auto texture = GetCachedImage(path, timestamp); texture != nullptr)
					continue;

				m_Pool.enqueue([this, path, timestamp]
				{
					int width, height, channels;
					unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
					if (data)
					{
						std::lock_guard<std::mutex> lock(m_ProcessedMutex);

						TextureSpecification spec;
						spec.Width = width;
						spec.Height = height;
						switch (channels)
						{
							case 3:
								spec.Format = ImageFormat::RGB8;
								break;
							case 4:
								spec.Format = ImageFormat::RGBA8;
								break;
						}

						float aspectratioW = (float)width / (float)height;
						float aspectratioH = (float)height / (float)width;
						int newWidth = clamp<int>((int)(m_ThumbnailSize * aspectratioW), 1, m_ThumbnailSize);
						int newHeight = clamp<int>((int)(m_ThumbnailSize * aspectratioH), 1, m_ThumbnailSize);

						m_ProcessedQueue.push({ path, {data, spec, channels, newWidth, newHeight}, timestamp });
					}
					else
					{
						UNEYE_CORE_TRACE("Failed to load image from path: " + path.string());
					}

				});
			}
		}
	}

}
