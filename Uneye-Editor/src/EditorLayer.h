#pragma once

#include <Uneye.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Panels/SceneHierarchyPanel.h"

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
		void SaveSceneAs();

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

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}
