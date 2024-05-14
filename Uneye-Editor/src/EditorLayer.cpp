#include "EditorLayer.h"

#include <imgui/imgui.h>

#include "Uneye/Scene/SceneSerializer.h"
#include "Uneye/Utils/PlatformUtils.h"

#include <ImGuizmo.h>
#include "Uneye/Math/Math.h"



namespace Uneye
{
	void EditorLayer::OnAttach()
	{
		UNEYE_PROFILE_FUNCTION();

		//Application::Get().GetWindow().SetVSync(false);

		Uneye::FramebufferSpecification fbspec;
		fbspec.Attachments = {
			FramebufferTextureFormat::RGBA8, 
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::Depth 
		};
		fbspec.Width = 800;
		fbspec.Height = 600;
		m_Framebuffer = Uneye::Framebuffer::Create(fbspec);

		m_ActiveScene = CreateRef<Scene>();

		for (int y = 0; y < 5; y++)
		{
			for (int x = 0; x < 5; x++)
			{
				auto entt = m_ActiveScene->CreateEntity("Tile square " + std::to_string(x + y * 5));
				entt.GetComponent<TransformComponent>().Translation = glm::vec3(x * 0.6f, y * 0.6f, 0.0f);
				entt.GetComponent<TransformComponent>().Scale = glm::vec3(0.5f, 0.5f, 1.0f);
				entt.AddComponent<MaterialComponent>(glm::vec4(1.0f), "assets/textures/blending_transparent_window.png");
			}
		}

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(45.0f, 1.677, 0.1f, 1000.0f);

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		UNEYE_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		UNEYE_PROFILE_FUNCTION();

		if (Uneye::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		//if (m_ViewportFocused)
		//{
		//	m_EditorCamera.OnUpdate(ts);
		//}
		m_EditorCamera.OnUpdate(ts);

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::Clear(glm::vec4(0.1f, 0.1f, 0.13f, 1.0f));
		//RenderCommand::Clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		m_Framebuffer->ClearAttachment(1, -1);

		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 &&
			mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}


		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(UNEYE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(UNEYE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	void EditorLayer::OnImGuiRender()
	{
		UNEYE_PROFILE_FUNCTION();


		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit")) { Uneye::Application::Get().Close(); }

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };		m_ViewportFocused = ImGui::IsWindowFocused();
		
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportSize.x, viewportSize.y };
		uint32_t texID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)texID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
		

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GuizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y,
				m_ViewportBounds[1].x - m_ViewportBounds[0].x,
				m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Runtime Camera from entt
			//auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			//const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			//const glm::mat4& cameraProjection = camera.GetProjection();
			//glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			//Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();


			// Entity
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap for translation and scale
			// Snap for rotation
			if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GuizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);


			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation(0.0f), rotation(0.0f), scale(1.0f);
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;

			}

		}
		
		
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();

		//ImGui::ShowDemoWindow();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();

				break;
			}
			case Key::O:
			{
				if (control)
					OpenScene();

				break;
			}
			case Key::S:
			{
				if (control && shift)
					SaveSceneAs();

				break;
			}

			// Guizmo
			case Key::Q:
			{
				if (!ImGuizmo::IsUsing())
					m_GuizmoType = -1;
				break;
			}
			case Key::T:
			{
				if (!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case Key::R:
			{
				if (!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case Key::E:
			{
				if (!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
		}
	}
	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::Button_Left)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Uneye Scene (*.uneye)\0*.uneye\0");
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Uneye Scene (*.uneye)\0*.uneye\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}
}
