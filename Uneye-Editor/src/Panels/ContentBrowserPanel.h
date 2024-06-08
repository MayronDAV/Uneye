#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Renderer/Texture.h"

#include <filesystem>
#include "ThumbnailCache.h"


namespace Uneye
{

	class ContentBrowserPanel
	{
		public:
			ContentBrowserPanel(Ref<Project> p_project);

			void OnImGuiRender();

		private:
			void ShowDirectoryTree(const std::filesystem::path& p_path);

			void ContentTopSearch();
			void ContentTree();
			void ContentBrowser();
			void ContentSearchResult();

		private:
			float m_SplitterWidth = 8.0f;
			float m_LeftPanelWidth = 150.0f;
			float m_Padding = 16.0f;
			float m_ThumbnailSize = 64.0f;

		private:
			Ref<Project> m_Project;
			Ref<ThumbnailCache> m_ThumbnailCache;

			std::filesystem::path m_BaseDirectory;
			std::filesystem::path m_CurrentDirectory;

			Ref<Texture2D> m_DirectoryIcon;
			Ref<Texture2D> m_FileIcon;

	};

}
