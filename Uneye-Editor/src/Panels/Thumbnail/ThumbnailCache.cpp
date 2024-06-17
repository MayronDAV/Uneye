#include "ThumbnailCache.h"

#include "Uneye/Asset/TextureImporter.h"
#include "Uneye/Core/CompressionManager.h"

#include <chrono>

#include <iostream>
#include <string>
#include <fstream>

#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>



namespace Uneye 
{
	static std::unordered_set<std::string_view> s_TextureExtensions = {
		".png", ".jpg", ".jpeg"
	};

	ThumbnailCache::ThumbnailCache(Ref<Project> project)
		: m_Project(project), m_ThumbnailCachePath(project->GetCacheDirectory() / "Thumbnail.cache"),
		m_StopThread(false), m_Pool(4), m_WorkerThread(&ThumbnailCache::ProcessQueue, this), 
		m_SaveLoadThread(&ThumbnailCache::ProcessSaveLoadQueue, this), m_SaveTimerThread(&ThumbnailCache::PeriodicSave, this)
	{
		LoadCachedImages();
	}

	ThumbnailCache::~ThumbnailCache()
	{
		{
			std::unique_lock<std::mutex> lock1(m_CachedImagesMutex);
			std::unique_lock<std::mutex> lock2(m_QueueMutex);
			std::unique_lock<std::mutex> lock3(m_ProcessedMutex);
			std::unique_lock<std::mutex> lock4(m_SaveLoadMutex);
			std::unique_lock<std::mutex> lock5(m_SaveTimerMutex);
			m_StopThread = true;
		}

		m_Condition.notify_all();
		m_SaveLoadCondition.notify_all();
		m_SaveTimerCondition.notify_all();

		if (m_WorkerThread.joinable())
			m_WorkerThread.join();
		if (m_SaveLoadThread.joinable())
			m_SaveLoadThread.join();
		if (m_SaveTimerThread.joinable())
			m_SaveTimerThread.join();

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
			if (m_QueuedItems.find(path) != m_QueuedItems.end()) m_QueuedItems.erase(path);
			m_ProcessedQueue.pop();
		}
	}

	void ThumbnailCache::SaveCachedImages()
	{
		UNEYE_CORE_INFO("Saving thumbnail cache...");

		std::lock_guard<std::mutex> lock(m_ProcessedMutex);
		std::vector<unsigned char> buffer;

		uint64_t numImages = m_CachedImages.size();
		buffer.insert(buffer.end(), reinterpret_cast<unsigned char*>(&numImages), reinterpret_cast<unsigned char*>(&numImages) + sizeof(numImages));

		for (const auto& [path, cachedImage] : m_CachedImages)
		{
			std::string pathStr = path.string();
			uint64_t pathSize = pathStr.size();
			buffer.insert(buffer.end(), reinterpret_cast<unsigned char*>(&pathSize), reinterpret_cast<unsigned char*>(&pathSize) + sizeof(pathSize));
			buffer.insert(buffer.end(), pathStr.begin(), pathStr.end());

			uint64_t timestamp = cachedImage.Timestamp;
			buffer.insert(buffer.end(), reinterpret_cast<unsigned char*>(&timestamp), reinterpret_cast<unsigned char*>(&timestamp) + sizeof(timestamp));

			std::vector<unsigned char> imageData;
			uint32_t width = cachedImage.Image->GetWidth();
			uint32_t height = cachedImage.Image->GetHeight();
			uint32_t channels = cachedImage.Image->GetChannels();
			imageData.insert(imageData.end(), reinterpret_cast<unsigned char*>(&width), reinterpret_cast<unsigned char*>(&width) + sizeof(width));
			imageData.insert(imageData.end(), reinterpret_cast<unsigned char*>(&height), reinterpret_cast<unsigned char*>(&height) + sizeof(height));
			imageData.insert(imageData.end(), reinterpret_cast<unsigned char*>(&channels), reinterpret_cast<unsigned char*>(&channels) + sizeof(channels));
			imageData.insert(imageData.end(), cachedImage.Image->GetData().begin(), cachedImage.Image->GetData().end());

			std::vector<unsigned char> compressedData = CompressionManager::Compress(imageData);

			uint64_t compressedSize = compressedData.size();
			uint64_t originalSize = imageData.size();
			buffer.insert(buffer.end(), reinterpret_cast<unsigned char*>(&originalSize), reinterpret_cast<unsigned char*>(&originalSize) + sizeof(originalSize));
			buffer.insert(buffer.end(), reinterpret_cast<unsigned char*>(&compressedSize), reinterpret_cast<unsigned char*>(&compressedSize) + sizeof(compressedSize));
			buffer.insert(buffer.end(), compressedData.begin(), compressedData.end());
		}

		std::vector<unsigned char> compressedBuffer = CompressionManager::Compress(buffer);

		std::ofstream outFile(m_ThumbnailCachePath, std::ios::binary);
		if (!outFile)
		{
			UNEYE_CORE_ERROR("Failed to save thumbnail cache");
			return;
		}


		uint64_t originalSize = buffer.size();

		outFile.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));

		outFile.write(reinterpret_cast<const char*>(compressedBuffer.data()), compressedBuffer.size());
		outFile.close();

		UNEYE_CORE_INFO("Finished!!!");
	}

	void ThumbnailCache::LoadCachedImages()
	{
		std::ifstream inFile(m_ThumbnailCachePath, std::ios::binary);
		if (!inFile)
		{
			UNEYE_CORE_ERROR("Failed to open thumbnail cache file for reading");
			return;
		}

		UNEYE_CORE_INFO("Loading thumbnail cache...");

		uint64_t originalSize;
		inFile.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));

		std::vector<unsigned char> compressedBuffer((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
		inFile.close();

		std::vector<unsigned char> buffer = CompressionManager::Decompress(compressedBuffer, originalSize);

		if (buffer.size() != originalSize)
		{
			UNEYE_CORE_ERROR("Decompressed buffer size does not match the original size");
			return;
		}

		size_t offset = 0;

		uint64_t numImages = *reinterpret_cast<const uint64_t*>(&buffer[offset]);
		offset += sizeof(numImages);

		for (uint64_t i = 0; i < numImages; ++i)
		{
			uint64_t pathSize = *reinterpret_cast<const uint64_t*>(&buffer[offset]);
			offset += sizeof(pathSize);

			std::string pathStr(buffer.begin() + offset, buffer.begin() + offset + pathSize);
			offset += pathSize;
			std::filesystem::path path(pathStr);

			uint64_t timestamp = *reinterpret_cast<const uint64_t*>(&buffer[offset]);
			offset += sizeof(timestamp);

			uint64_t imgOriginalSize = *reinterpret_cast<const uint64_t*>(&buffer[offset]);
			offset += sizeof(imgOriginalSize);
			uint64_t compressedSize = *reinterpret_cast<const uint64_t*>(&buffer[offset]);
			offset += sizeof(compressedSize);

			std::vector<unsigned char> compressedData(buffer.begin() + offset, buffer.begin() + offset + compressedSize);
			offset += compressedSize;

			std::vector<unsigned char> imageData = CompressionManager::Decompress(compressedData, imgOriginalSize);

			if (imageData.size() != imgOriginalSize)
			{
				UNEYE_CORE_ERROR("Decompressed image data size does not match the original size for path: " + pathStr);
				continue;
			}

			uint32_t width, height, channels;
			std::memcpy(&width, &imageData[0], sizeof(width));
			std::memcpy(&height, &imageData[sizeof(width)], sizeof(height));
			std::memcpy(&channels, &imageData[sizeof(width) + sizeof(height)], sizeof(channels));

			unsigned char* imageDataRaw = imageData.data() + sizeof(width) + sizeof(height) + sizeof(channels);

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

			Ref<Texture2D> texture = Texture2D::Create(spec, { imageDataRaw, (uint64_t)(width * height * channels) });
			if (texture)
			{
				float aspectratioW = (float)width / (float)height;
				float aspectratioH = (float)height / (float)width;
				uint32_t newWidth = clamp<int>((int)(m_ThumbnailSize * aspectratioW), 1, m_ThumbnailSize);
				uint32_t newHeight = clamp<int>((int)(m_ThumbnailSize * aspectratioH), 1, m_ThumbnailSize);
				texture->ChangeSize(newWidth, newHeight);

				auto& cachedImage = m_CachedImages[path];
				cachedImage.Timestamp = timestamp;
				cachedImage.Image = texture;
			}
		}

		UNEYE_CORE_INFO("Finished!!!");
	}


	void ThumbnailCache::SaveCachedImagesAsync()
	{
		AddToSaveLoadQueue([this] { SaveCachedImages(); });
	}


	void ThumbnailCache::AddToQueue(const std::filesystem::path& path, uint64_t timestamp)
	{
		{
			std::lock_guard<std::mutex> lock(m_QueueMutex);
			if (m_QueuedItems.find(path) == m_QueuedItems.end())
			{
				m_Queue.push({ path, timestamp });
				m_QueuedItems.insert(path);
			}
		}
		m_Condition.notify_one();
	}

	void ThumbnailCache::AddToSaveLoadQueue(const std::function<void()>& task)
	{
		{
			std::lock_guard<std::mutex> lock(m_SaveLoadMutex);
			m_SaveLoadQueue.push(task);
		}
		m_SaveLoadCondition.notify_one();
	}

	Ref<Texture2D> ThumbnailCache::GetCachedImage(const std::filesystem::path& path, uint64_t timestamp)
	{
		std::lock_guard<std::mutex> lock(m_CachedImagesMutex);
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
			std::unique_lock<std::mutex> lock(m_QueueMutex);
			m_Condition.wait(lock, [this] { return !m_Queue.empty() || m_StopThread; });

			if (m_StopThread && m_Queue.empty())
				break;

			if (!m_Queue.empty() && m_Queue.size() > 0)
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

	void ThumbnailCache::ProcessSaveLoadQueue()
	{
		while (true)
		{
			std::unique_lock<std::mutex> lock(m_SaveLoadMutex);
			m_SaveLoadCondition.wait(lock, [this] { return !m_SaveLoadQueue.empty() || m_StopThread; });

			if (m_StopThread && m_SaveLoadQueue.empty())
				break;

			if (!m_SaveLoadQueue.empty() && m_SaveLoadQueue.size() > 0)
			{
				auto task = std::move(m_SaveLoadQueue.front());
				m_SaveLoadQueue.pop();
				lock.unlock();

				task();
			}
		}
	}

	void ThumbnailCache::PeriodicSave()
	{
		while (true)
		{
			std::unique_lock<std::mutex> lock(m_SaveTimerMutex);
			if (m_SaveTimerCondition.wait_for(lock, std::chrono::minutes(m_PeriodicSaveTime), [this] { return m_StopThread; }))
				break;

			SaveCachedImagesAsync();
		}
	}

}
