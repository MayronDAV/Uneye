#pragma once

#include <Uneye.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Uneye/Renderer/EditorCamera.h"



namespace Uneye
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer() :Layer("EditorLayer") {}
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender() override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneStop();

		// UI Panels
		void UI_Toolbar();

	private:
		// Temp
		Ref<Shader> m_Shader;
		Ref<VertexArray> m_VertexArray;

		Entity m_HoveredEntity;

		Uneye::Ref<Uneye::Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;

		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GuizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconStop;
	};
}
