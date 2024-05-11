#pragma once

#include <Uneye.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Panels/SceneHierarchyPanel.h"




namespace Uneye
{
	class EditorLayer : public Layer
	{
		public:
			EditorLayer() :Layer("EditorLayer"), m_CameraController(800.0f / 600.0f) {}
			virtual ~EditorLayer() = default;

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			virtual void OnUpdate(Timestep ts) override;
			virtual void OnEvent(Event& e) override;
			virtual void OnImGuiRender() override;

		private:
			bool OnKeyPressed(KeyPressedEvent& e);

			void NewScene();
			void OpenScene();
			void SaveSceneAs();

		private:
			Uneye::OrthographicCameraController m_CameraController;

			// Temp
			Ref<Shader> m_Shader;
			Ref<VertexArray> m_VertexArray;
			Ref<Texture2D> m_Texture;
			Uneye::Ref<Uneye::Framebuffer> m_Framebuffer;

			Ref<Scene> m_ActiveScene;
			Entity m_SquareEntity;
			Entity m_CameraEntity;
			Entity m_SecondCamera;

			bool m_PrimaryCamera = true;

			glm::vec2 m_ViewportSize{0.0f, 0.0f};
			bool m_ViewportFocused = false;
			bool m_ViewportHovered = false;

			glm::vec4 m_SquareColor{ 0.2f, 0.3f, 0.8f, 1.0f };

			// Panels
			SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}
