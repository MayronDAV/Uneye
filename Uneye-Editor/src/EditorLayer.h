#pragma once

#include <Uneye.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/AssetImporterPanel.h"
#include "Panels/LogPanel.h"

#include "Uneye/Renderer/EditorCamera.h"



namespace Uneye
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender() override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnWindowDrop(WindowDropEvent& e);

		void OnOverlayRender();

		void ReloadAssembly();

		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(AssetHandle handle);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnScenePause();
		void OnSceneStop();

		void OnSceneSimulate();

		void OnDuplicateEntity();
		void OnDestroyEntity();

		// UI Panels
		void UI_Toolbar();

	private:
		// Temp
		Ref<Shader> m_Shader;
		Ref<VertexArray> m_VertexArray;

		Entity m_HoveredEntity;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditScenePath;

		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GuizmoType = -1;

		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		Scope<ContentBrowserPanel> m_ContentBrowserPanel;
		bool m_AssetImporterPanelIsOpen = false;
		AssetImporterPanel m_AssetImporterPanel;
		LogPanel m_LogPanel;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStop, m_IconStep, m_IconSimulate;
	};
}
