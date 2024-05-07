#include "EditorLayer.h"

#include <imgui/imgui.h>

namespace Uneye
{
	void EditorLayer::OnAttach()
	{
		UNEYE_PROFILE_FUNCTION();

		//Application::Get().GetWindow().SetVSync(false);
		//m_Texture = Texture2D::Create("assets/textures/wall.jpg");
		m_Texture = Uneye::Texture2D::Create("assets/game/RPGpack_sheet_2X.png");

		Uneye::FramebufferSpecification fbspec;
		fbspec.Width = 800;
		fbspec.Height = 600;
		m_Framebuffer = Uneye::Framebuffer::Create(fbspec);

		m_ActiveScene = CreateRef<Scene>();
		auto square = m_ActiveScene->CreateEntity("Green Square");
		square.AddComponent<SpriteComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_SquareEntity = square;
		
		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();
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
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		if (m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		//RenderCommand::Clear(glm::vec4(0.1f, 0.1f, 0.13f, 1.0f));
		RenderCommand::Clear(glm::vec4(1.0f));

		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
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
				if (ImGui::MenuItem("Exit")) { Uneye::Application::Get().Close(); }

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Setup");
		if (m_SquareEntity)
		{
			auto tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
			if (ImGui::CollapsingHeader(tag.c_str()))
			{
				ImGui::Separator();
				auto& squareColor = m_SquareEntity.GetComponent<SpriteComponent>().Color;
				ImGui::ColorEdit4("Color", glm::value_ptr(squareColor));
				ImGui::Separator();
			}

			tag = m_CameraEntity.GetComponent<TagComponent>().Tag;
			if (ImGui::CollapsingHeader(tag.c_str()))
			{
				ImGui::Separator();
				auto& transform = m_CameraEntity.GetComponent<TransformComponent>().Transform[3];
				ImGui::DragFloat3("Transform", glm::value_ptr(transform));
				ImGui::Separator();

				auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
				float orthoSize = camera.GetOrthographicSize();
				if( ImGui::DragFloat("Ortho size", &orthoSize) )
				{
					camera.SetOrthographicSize(orthoSize);
				}
				ImGui::Separator();
			}
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		
		//if ()
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportSize.x, viewportSize.y };
		uint32_t texID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)texID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Quads Setup");
		auto stats = Uneye::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats: ");
		ImGui::Text("Draw Calls:	%d", stats.DrawCalls);
		ImGui::Text("Quad Count:	%d", stats.QuadCount);
		ImGui::Text("Vertices:		%d", stats.GetTotalVertexCount());
		ImGui::Text("Indices:		%d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::End();

		//ImGui::ShowDemoWindow();
	}
}