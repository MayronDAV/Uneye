#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Core/Log.h"

#include "Uneye/Asset/RuntimeAssetManager.h"
#include "Uneye/Asset/EditorAssetManager.h"

#include <string>
#include <filesystem>


namespace Uneye
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		AssetHandle StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path CacheDirectory;
		std::filesystem::path AssetRegistryPath;
		std::filesystem::path ScriptModulePath;

		// TODO: A list of all scenes provided.
		// std::vector<std::filesystem::path> ScenesPath;
	};

	class Project
	{
		public:
			
			const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }
			std::filesystem::path GetAssetDirectory()
			{
				return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
			}
			std::filesystem::path GetCacheDirectory()
			{
				return GetAssetDirectory() / s_ActiveProject->m_Config.CacheDirectory;
			}
			std::filesystem::path GetAssetRegistryPath()
			{
				return GetAssetDirectory() / s_ActiveProject->m_Config.AssetRegistryPath;
			}
			// TODO: Move to Asset manager
			std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }

			std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& path)
			{
				return GetAssetDirectory() / path;
			}

			static const std::filesystem::path& GetActiveProjectDirectory()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->GetProjectDirectory();
			}

			static const std::filesystem::path& GetActiveProjectFile()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->m_ProjectFile;
			}

			static std::filesystem::path GetActiveAssetDirectory()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->GetAssetDirectory();
			}

			static std::filesystem::path GetActiveCacheDirectory()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->GetCacheDirectory();
			}

			static std::filesystem::path GetActiveAssetRegistryPath()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->GetAssetRegistryPath();
			}

			// TODO: move to asset manager when have one
			static std::filesystem::path GetActiveAssetFileSystemPath(const std::filesystem::path& path)
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->GetAssetFileSystemPath(path);
			}

			static AssetHandle& GetStartScene()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->m_Config.StartScene;
			}

			static void SetStartScene(AssetHandle p_handle)
			{
				s_ActiveProject->m_Config.StartScene = p_handle;
			}

			static std::filesystem::path& GetScriptModulePath()
			{
				UNEYE_CORE_ASSERT(!s_ActiveProject);
				return s_ActiveProject->m_Config.ScriptModulePath;
			}



			ProjectConfig& GetConfig() { return m_Config; }

			static Ref<Project> GetActive() { return s_ActiveProject;	}

			std::shared_ptr<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
			std::shared_ptr<RuntimeAssetManager> GetRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManager); }
			std::shared_ptr<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }


			static Ref<Project> New();
			static Ref<Project> Load(const std::filesystem::path& path);
			static bool SaveActive(const std::filesystem::path& path);

		private:
			ProjectConfig m_Config;
			std::filesystem::path m_ProjectDirectory;
			std::filesystem::path m_ProjectFile;
			std::shared_ptr<AssetManagerBase> m_AssetManager;

			inline static Ref<Project> s_ActiveProject;
	};
}
