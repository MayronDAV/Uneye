#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Renderer/Texture.h"

#include <filesystem>


namespace Uneye
{

	class ContentBrowserPanel
	{
		public:
			ContentBrowserPanel();

			void OnImGuiRender();

		private:
			void ShowDirectoryTree(const std::filesystem::path& path);

		private:
			std::filesystem::path m_CurrentDirectory;

			Ref<Texture2D> m_DirectoryIcon;
			Ref<Texture2D> m_FileIcon;
	};

}
