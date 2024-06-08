#pragma once

#include "Uneye/Project/Project.h"
#include "Uneye/Renderer/Texture.h"

#include "Uneye/Core/ThreadPool.h"

#include <queue>
#include <unordered_set>



namespace Uneye 
{
	struct ThumbnailImage
	{
		uint64_t Timestamp;
		Ref<Texture2D> Image;
	};

	struct ThumbInfo
	{
		std::filesystem::path Path;
		uint64_t Timestamp;
	};

	struct TextureInfo
	{
		unsigned char* Data;
		TextureSpecification Spec;
		int Channels;

		int newWidth;
		int newHeight;
	};

	class ThumbnailCache
	{
		public:

			ThumbnailCache(Ref<Project> project);
			~ThumbnailCache();

			Ref<Texture2D> GetOrCreateThumbnail(const std::filesystem::path& path);

			void OnUpdate();

		private:

			// Maybe move this somewhere with other math functions ?
			template <typename T>
			T clamp(T value, T min, T max) { return (value < min) ? min : (value > max) ? max : value; }

			void AddToQueue(const std::filesystem::path& path, uint64_t timestamp);

			Ref<Texture2D> GetCachedImage(const std::filesystem::path& path, uint64_t timestamp);

			void ProcessQueue();

		private:

			std::queue<std::tuple<std::filesystem::path, TextureInfo, uint64_t>> m_ProcessedQueue;

			std::mutex m_Mutex;
			std::mutex m_ProcessedMutex;
			std::condition_variable m_Condition;
			ThreadPool m_Pool;
			std::thread m_WorkerThread;
			bool m_StopThread;

			uint32_t m_ThumbnailSize = 128;

			Ref<Project> m_Project;

			std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;
			std::queue<ThumbInfo> m_Queue;
			std::unordered_set<std::filesystem::path> m_QueuedItems;

			// TEMP (replace with Uneye::Serialization)
			std::filesystem::path m_ThumbnailCachePath;
	};

}
