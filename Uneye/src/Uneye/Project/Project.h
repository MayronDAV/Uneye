#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Core/Log.h"

#include <string>
#include <filesystem>


namespace Uneye
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		// Maybe change this to be a list of all asset directories provided.
		std::filesystem::path AssetDirectory;
		// Maybe change this to be a list of all scrípt directories provided.
		std::filesystem::path ScriptModulePath;

		// TODO: A list of all scenes provided.
		// std::vector<std::filesystem::path> ScenesPath;
	};

	class Project
	{
		public:
			
			static std::filesystem::path& GetStartScene()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->m_Config.StartScene;
			}

			// Maybe change this to return a list of all asset directories provided.
			static std::filesystem::path GetAssetDirectory()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
			}

			// Maybe change this to return a list of all scrípt directories provided.
			static std::filesystem::path& GetScriptModulePath()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->m_Config.ScriptModulePath;
			}

			static const std::filesystem::path& GetProjectDirectory() 
			{ 
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->m_ProjectDirectory;
			}

			// Maybe change this to return a list of all asset directories provided.
			static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return GetAssetDirectory() / path;
			}

			static Ref<Project> GetActive() { return s_ActiveProject;	}

			ProjectConfig& GetConfig() { return m_Config; }

			static Ref<Project> New();
			static Ref<Project> Load(const std::filesystem::path& path);
			static bool SaveActive(const std::filesystem::path& path);

		private:
			ProjectConfig m_Config;
			std::filesystem::path m_ProjectDirectory;

			inline static Ref<Project> s_ActiveProject;
	};
}
