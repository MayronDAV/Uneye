#include "uypch.h"
#include "SceneManager.h"

#include "Uneye/Asset/AssetManager.h"


#include <memory>


namespace Uneye
{
	struct Data
	{
		glm::vec2 ViewportSize;

		SceneState State;

		int Timerscale;
		LoadMode LoadMode;

		Ref<Scene> ActiveScene;
		Ref<Scene> EditorScene;
		std::filesystem::path CurrentScenePath;

		std::unordered_map<std::filesystem::path, Ref<Scene>> ActiveScenesByPath;
		std::unordered_map<std::filesystem::path, Ref<Scene>> EditorScenesByPath;
		std::vector<Ref<Scene>> ScenesNotLoaded;

		EditorCamera EditorCamera;
	};

	static Data s_Data;


	void SceneManager::SetTimerscale(int p_scale)
	{
		s_Data.Timerscale = p_scale;
	}

	void SceneManager::Init()
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data.ViewportSize.x = 800;
		s_Data.ViewportSize.y = 600;

		s_Data.State = SceneState::Edit;

		s_Data.Timerscale = 1;
		s_Data.LoadMode = LoadMode::Single;

		s_Data.EditorScene = CreateRef<Scene>();
		s_Data.ActiveScene = s_Data.EditorScene;

		s_Data.EditorCamera = EditorCamera(45.0f, 1.677, 0.1f, 1000.0f);
	}

	void SceneManager::Shutdown()
	{
		UNEYE_PROFILE_FUNCTION();
	}

	SceneManager::~SceneManager()
	{
		UNEYE_PROFILE_FUNCTION();

		Shutdown();
	}

	void SceneManager::SetLoadMode(LoadMode p_mode)
	{
		s_Data.LoadMode = p_mode;
	}

	bool SceneManager::LoadScene(const AssetHandle& p_handle, LoadMode p_mode)
	{
		if (!AssetManager::IsAssetHandleValid(p_handle))
		{
			UNEYE_CORE_ERROR("Invalid handle, make sure you import the scene before passing");
			return false;
		}

		auto metadata = AssetManager::GetAssetRegistry().at(p_handle);

		return LoadScene(metadata.FilePath.string(), p_mode);
	}

	bool SceneManager::LoadScene(const std::string& p_path, LoadMode p_mode)
	{
		UNEYE_PROFILE_FUNCTION();

		auto [handle, metadata] = AssetManager::GetHandleByPath(p_path);
		auto scene = AssetManager::GetAsset<Scene>(handle);
		if (!scene)
		{
			UNEYE_CORE_ERROR("Invalid path");
			return false;
		}

		s_Data.LoadMode = p_mode;

		if (s_Data.LoadMode == LoadMode::Single)
		{
			s_Data.EditorScene = scene;
			s_Data.EditorScene->Handle = handle;

			s_Data.EditorScenesByPath[p_path] = scene;
			s_Data.EditorScenesByPath[p_path]->Handle = handle;
		}

		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			s_Data.EditorScenesByPath[p_path] = scene;
			s_Data.EditorScenesByPath[p_path]->Handle = handle;
		}

		return true;
	}

	const std::filesystem::path& SceneManager::GetCurrentScenePath()
	{
		return s_Data.CurrentScenePath;
	}

	Ref<Scene> SceneManager::GetActiveScene()
	{
		if (s_Data.LoadMode != LoadMode::Single)
		{
			UNEYE_CORE_ERROR("Call GetAllActiveScenes in LoadMode::Additive");
			return Ref<Scene>();
		}

		return s_Data.ActiveScene;
	}

	std::unordered_map<std::filesystem::path, Ref<Scene>> SceneManager::GetAllActiveScenes()
	{
		if (s_Data.LoadMode != LoadMode::Additive)
		{
			UNEYE_CORE_ERROR("Call GetActiveScene in LoadMode::Single");
			return std::unordered_map<std::filesystem::path, Ref<Scene>>();
		}

		return s_Data.ActiveScenesByPath;
	}


	void SceneManager::OnScenePlay()
	{
		if (s_Data.State == SceneState::Simulate)
			OnSceneStop();

		s_Data.State = SceneState::Play;

		if (s_Data.LoadMode == LoadMode::Single)
		{
			s_Data.ActiveScene = Scene::Copy(s_Data.EditorScene);
			s_Data.ActiveScene->OnRuntimeStart();
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, editorScene] : s_Data.EditorScenesByPath)
			{
				auto scene = Scene::Copy(editorScene);
				s_Data.ActiveScenesByPath[path] = scene;
				scene->OnRuntimeStart();
			}
		}

		//TODO::
		//m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	}

	void SceneManager::OnSceneStop()
	{
		// Revisit this

		UNEYE_ASSERT(s_Data.State != SceneState::Play && s_Data.State != SceneState::Simulate, "Unknown state!");

		if (s_Data.LoadMode == LoadMode::Single)
		{
			if (s_Data.State == SceneState::Play)
				s_Data.ActiveScene->OnRuntimeStop();
			else if (s_Data.State == SceneState::Simulate)
				s_Data.ActiveScene->OnSimulationStop();

			s_Data.State = SceneState::Edit;
			s_Data.ActiveScene = s_Data.EditorScene;
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, scene] : s_Data.ActiveScenesByPath)
			{
				if (s_Data.State == SceneState::Play)
					scene->OnRuntimeStop();
				else if (s_Data.State == SceneState::Simulate)
					scene->OnSimulationStop();

				s_Data.State = SceneState::Edit;
				s_Data.ActiveScenesByPath[path] = s_Data.EditorScenesByPath[path];
			}
		}

		// TODO:
		//m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void SceneManager::OnScenePause(bool pause)
	{
		if (s_Data.State == SceneState::Edit)
			return;

		if (s_Data.LoadMode == LoadMode::Single)
		{
			s_Data.ActiveScene->SetPaused(pause);
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, scene] : s_Data.ActiveScenesByPath)
			{
				scene->SetPaused(pause);
			}
		}
	}

	void SceneManager::OnSceneSimulate()
	{
		if (s_Data.State == SceneState::Play)
			OnSceneStop();

		s_Data.State = SceneState::Simulate;

		if (s_Data.LoadMode == LoadMode::Single)
		{
			s_Data.ActiveScene = Scene::Copy(s_Data.EditorScene);
			s_Data.ActiveScene->OnSimulationStart();
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, editorScene] : s_Data.EditorScenesByPath)
			{
				auto scene = Scene::Copy(editorScene);
				s_Data.ActiveScenesByPath[path] = scene;
				scene->OnSimulationStart();
			}
		}

		// TODOD:
		//m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void SceneManager::OnEditorCameraEvent(Event& e)
	{
		if (s_Data.State == SceneState::Edit)
			s_Data.EditorCamera.OnEvent(e);
	}

	const EditorCamera& SceneManager::GetEditorCamera()
	{
		return s_Data.EditorCamera;
	}

	SceneState SceneManager::GetState()
	{
		return s_Data.State;
	}

	void SceneManager::Resize(uint32_t width, uint32_t height)
	{
		if (s_Data.LoadMode == LoadMode::Single)
		{
			s_Data.ActiveScene->OnViewportResize(width, height);
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, scene] : s_Data.ActiveScenesByPath)
			{
				scene->OnViewportResize(width, height);
			}
		}

		if (width > 0 && height > 0 && 
			width != s_Data.ViewportSize.x && height != s_Data.ViewportSize.y)
		{
			s_Data.ViewportSize.x = width;
			s_Data.ViewportSize.y = height;


			s_Data.EditorCamera.SetViewportSize(s_Data.ViewportSize.x, s_Data.ViewportSize.y);
		}
	}

	const Ref<Scene>& SceneManager::GetEditorScene()
	{
		if (s_Data.LoadMode != LoadMode::Single)
		{
			UNEYE_CORE_ERROR("Call GetAllEditorScenes in LoadMode::Additive");
			return nullptr;
		}

		return s_Data.EditorScene;
	}

	const AssetHandle& SceneManager::NewScene()
	{
		// Revisit this

		auto scene = CreateRef<Scene>();

		if (s_Data.LoadMode == LoadMode::Single)
		{
			if (!s_Data.ActiveScene)
			{
				UNEYE_CORE_CRITICAL("Invalid scene!!!");
				return scene->Handle;
			}

			s_Data.ActiveScene = CreateRef<Scene>();
			s_Data.ActiveScene->OnViewportResize((uint32_t)s_Data.ViewportSize.x, (uint32_t)s_Data.ViewportSize.y);
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			scene->OnViewportResize((uint32_t)s_Data.ViewportSize.x, (uint32_t)s_Data.ViewportSize.y);
			s_Data.ScenesNotLoaded.push_back(scene);
		}
		
		return scene->Handle;
		// TODO: 
		//m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void SceneManager::SaveScene()
	{
		if (s_Data.LoadMode != LoadMode::Single)
		{
			UNEYE_CORE_ERROR("Call SaveScene with handle for LoadMode::Additive!");
			return;
		}

		SceneImporter::SaveScene(s_Data.ActiveScene, s_Data.CurrentScenePath);
	}

	void SceneManager::SaveScene(const AssetHandle& p_handle)
	{
		if (!AssetManager::IsAssetHandleValid(p_handle))
		{
			UNEYE_CORE_ERROR("Invalid handle, make sure you import the scene before passing");
			return;
		}

		auto scene = AssetManager::GetAsset<Scene>(p_handle);
		auto metadata = AssetManager::GetAssetRegistry().at(p_handle);

		SceneImporter::SaveScene(scene, metadata.FilePath);
	}

	void SceneManager::SaveAllScenes()
	{
		UNEYE_CORE_ASSERT(true, "Not implemented!");
	}

	void SceneManager::Play()
	{
		if (s_Data.State == SceneState::Edit || s_Data.State == SceneState::Simulate)
			OnScenePlay();
		else if (s_Data.State == SceneState::Play)
			OnSceneStop();
	}

	void SceneManager::Simulate()
	{
		if (s_Data.State == SceneState::Edit || s_Data.State == SceneState::Play)
			OnSceneSimulate();
		else if (s_Data.State == SceneState::Simulate)
			OnSceneStop();
	}

	void SceneManager::Pause(bool pause)
	{
		OnScenePause(pause);
	}

	void SceneManager::Step(int step)
	{
		if (s_Data.LoadMode == LoadMode::Single)
		{
			s_Data.ActiveScene->Step(step);
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, scene] : s_Data.ActiveScenesByPath)
			{
				scene->Step(step);
			}
		}
	}

	void SceneManager::Stop()
	{
		OnSceneStop();
	}


	void SceneManager::OnUpdate(Timestep p_ts)
	{
		switch (s_Data.State)
		{
			case SceneState::Edit:
				OnUpdateEditor(p_ts);
				break;
			case SceneState::Simulate:
				OnUpdateSimulate(p_ts);
				break;
			case SceneState::Play:
				OnUpdateRuntime(p_ts);
				break;
		}

	}

	void SceneManager::OnUpdateRuntime(Timestep p_ts)
	{
		UNEYE_PROFILE_FUNCTION();

		auto timer = p_ts * s_Data.Timerscale;

		if (s_Data.LoadMode == LoadMode::Single)
		{
			if (!s_Data.ActiveScene)
			{
				UNEYE_CORE_CRITICAL("Invalid scene!!!");
				return;
			}

			s_Data.ActiveScene->OnUpdateRuntime(p_ts);
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, scene] : s_Data.ActiveScenesByPath)
			{
				scene->OnUpdateRuntime(p_ts);
			}
		}
	}

	void SceneManager::OnUpdateEditor(Timestep p_ts)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data.EditorCamera.OnUpdate(p_ts);

		if (s_Data.LoadMode == LoadMode::Single)
		{
			if (!s_Data.EditorScene)
			{
				UNEYE_CORE_CRITICAL("Invalid scene!!!");
				return;
			}

			s_Data.EditorScene->OnUpdateEditor(p_ts, s_Data.EditorCamera);
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, scene] : s_Data.EditorScenesByPath)
			{
				scene->OnUpdateEditor(p_ts, s_Data.EditorCamera);
			}
		}
	}

	void SceneManager::OnUpdateSimulate(Timestep p_ts)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data.EditorCamera.OnUpdate(p_ts);

		if (s_Data.LoadMode == LoadMode::Single)
		{
			if (!s_Data.ActiveScene)
			{
				UNEYE_CORE_CRITICAL("Invalid scene!!!");
				return;
			}

			s_Data.ActiveScene->OnUpdateSimulation(p_ts, s_Data.EditorCamera);
		}
		else if (s_Data.LoadMode == LoadMode::Additive)
		{
			for (const auto& [path, scene] : s_Data.ActiveScenesByPath)
			{
				scene->OnUpdateSimulation(p_ts, s_Data.EditorCamera);
			}
		}
	}
}
