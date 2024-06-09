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

			void SaveCachedImages();
			void LoadCachedImages();

			void SaveCachedImagesAsync();

		private:
			// Maybe move this somewhere with other math functions ?
			template <typename T>
			T clamp(T value, T min, T max) { return (value < min) ? min : (value > max) ? max : value; }

			void AddToQueue(const std::filesystem::path& path, uint64_t timestamp);
			void AddToSaveLoadQueue(const std::function<void()>& task);

			Ref<Texture2D> GetCachedImage(const std::filesystem::path& path, uint64_t timestamp);

			void ProcessQueue();
			void ProcessSaveLoadQueue();

			void PeriodicSave();


		private:

			std::mutex m_Mutex;
			std::mutex m_ProcessedMutex;
			std::mutex m_LoadImagesMutex;
			std::mutex m_SaveTimerMutex;
			std::condition_variable m_Condition;
			std::condition_variable m_SaveLoadCondition;
			std::condition_variable m_SaveTimerCondition;

			ThreadPool m_Pool;
			std::thread m_WorkerThread;
			std::thread m_SaveLoadThread;
			std::thread m_SaveTimerThread;
			bool m_StopThread;

			uint32_t m_ThumbnailSize = 128;

			Ref<Project> m_Project;

			std::queue<std::tuple<std::filesystem::path, TextureInfo, uint64_t>> m_ProcessedQueue;
			std::queue<std::tuple<std::filesystem::path, TextureInfo, uint64_t>> m_LoadImagesQueue;
			std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;
			std::queue<ThumbInfo> m_Queue;
			std::unordered_set<std::filesystem::path> m_QueuedItems;
			std::queue<std::function<void()>> m_SaveLoadQueue;

			// TEMP (replace with Uneye::Serialization)
			std::filesystem::path m_ThumbnailCachePath;

			int m_PeriodicSaveTime = 1;
	};

}
