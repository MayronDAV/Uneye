#pragma once

#include "Uneye/Asset/Asset.h"
#include "Uneye/Core/Timestep.h"

#include "Uneye/Scene/Scene.h"
#include "Uneye/Asset/SceneImporter.h"
#include "Uneye/Events/Event.h"

// TODO: Make a way to serialize this SceneManager in editor/runtime mode.


namespace Uneye
{

	enum class LoadMode
	{
		Single = 0,
		Additive
	};

	enum class SceneState
	{
		Edit = 0, Play = 1, Simulate = 2
	};

	class SceneManager
	{
		public:
			SceneManager() = default;
			~SceneManager();

			static void Init();
			static void Shutdown();

			static void SetLoadMode(LoadMode p_mode);

			static bool LoadScene(const AssetHandle& p_handle, LoadMode p_mode = LoadMode::Single);
			static bool LoadScene(const std::string& p_path, LoadMode p_mode = LoadMode::Single);
			
			static const std::filesystem::path& GetCurrentScenePath();
			static Ref<Scene> GetActiveScene();
			static std::unordered_map<std::filesystem::path, Ref<Scene>> GetAllActiveScenes();

			// Runtime only

			static void SetTimerscale(int p_scale);


			// Editor only
			static void OnEditorCameraEvent(Event& e);
			static const EditorCamera& GetEditorCamera();
			static SceneState GetState();
			static void Resize(uint32_t width, uint32_t height);

			static const Ref<Scene>& GetEditorScene();

			static const AssetHandle& NewScene();
			static void SaveScene();
			static void SaveScene(const AssetHandle& p_handle);
			static void SaveAllScenes();

			static void Play();
			static void Simulate();
			static void Pause(bool pause);
			static void Step(int step);
			static void Stop();

			static void OnUpdate(Timestep p_ts);

		private:
			static void OnScenePlay();
			static void OnSceneStop();
			static void OnScenePause(bool pause);
			static void OnSceneSimulate();

			static void OnUpdateRuntime(Timestep p_ts);
			static void OnUpdateEditor(Timestep p_ts);
			static void OnUpdateSimulate(Timestep p_ts);
	};
}
