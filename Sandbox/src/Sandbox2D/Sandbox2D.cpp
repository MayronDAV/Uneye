#include "Sandbox2D.h"

#include <imgui/imgui.h>



void Sandbox2D::OnAttach()
{
	UNEYE_PROFILE_FUNCTION();

	//Uneye::Application::Get().GetWindow().SetVSync(false);
	m_Texture = Uneye::Texture2D::Create("assets/textures/wall.jpg");

	Uneye::FramebufferSpecification fbspec;
	fbspec.Width = 800;
	fbspec.Height = 600;
	m_Framebuffer = Uneye::Framebuffer::Create(fbspec);
}

void Sandbox2D::OnDetach()
{
	UNEYE_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Uneye::Timestep ts)
{
	UNEYE_PROFILE_FUNCTION();


	m_CameraController.OnUpdate(ts);

	Uneye::Renderer2D::ResetStats();
	m_Framebuffer->Bind();
	Uneye::RenderCommand::Clear(glm::vec4(1.0f));

	{
		UNEYE_PROFILE_SCOPE("Renderer Draw");
		Uneye::Renderer2D::BeginScene(m_CameraController.GetCamera());


		float Mapsize = 10;
		for (float x = 0.0f; x <= Mapsize; x += 1.0f)
		{
			for (float y = 0.0f; y <= Mapsize; y += 1.0f)
			{
				Uneye::Renderer2D::DrawQuad({ x * 0.11f, y * 0.11f, 0.01f },
					{ 0.1f, 0.1f }, { 0.8f, 0.3f, 0.2f, 1.0f });
			}
		}

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(-1.0f, 0.0f, 0.1f),
			{ 0.1f, 0.1f },
			m_SquareColor,
			Uneye::Texture2D::Create("assets/textures/container.jpg")
		);

		Uneye::Renderer2D::EndScene();

		Uneye::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float x = 0.0f; x <= Mapsize; x += 1.0f)
		{
			for (float y = 0.0f; y <= Mapsize; y += 1.0f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
				Uneye::Renderer2D::DrawQuad({ x * 0.11f, y * 0.11f, 0.1f },
					{ 0.1f, 0.1f }, color);
			}
		}

		Uneye::Renderer2D::DrawQuad(
			glm::vec3(-3.0f, 0.0f, 0.1f),
			{ 0.1f, 0.1f },
			m_SquareColor,
			Uneye::Texture2D::Create("assets/textures/container.jpg")
		);

		Uneye::Renderer2D::EndScene();
	}
	m_Framebuffer->Unbind();
}

void Sandbox2D::OnEvent(Uneye::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	UNEYE_PROFILE_FUNCTION();

	static bool dockspace = true;
	if (dockspace)
	{
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
		ImGui::Begin("DockSpace Demo", &dockspace, window_flags);
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
		ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
		ImGui::End();

		ImGui::Begin("Renderer");
		uint32_t texID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)texID, ImVec2(800.0f, 600.0f));
		ImGui::End();

		ImGui::Begin("Quads Setup");
		auto stats = Uneye::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats: ");
		ImGui::Text("Draw Calls:	%d", stats.DrawCalls);
		ImGui::Text("Quad Count:	%d", stats.QuadCount);
		ImGui::Text("Vertices:		%d", stats.GetTotalVertexCount());
		ImGui::Text("Indices:		%d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::End();
	}
	else
	{
		ImGui::Begin("Quads Setup");
		auto stats = Uneye::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats: ");
		ImGui::Text("Draw Calls:	%d", stats.DrawCalls);
		ImGui::Text("Quad Count:	%d", stats.QuadCount);
		ImGui::Text("Vertices:		%d", stats.GetTotalVertexCount());
		ImGui::Text("Indices:		%d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
		uint32_t texID = m_Texture->GetRendererID();
		ImGui::Image((void*)texID, ImVec2(64, 64));
		ImGui::End();
	}
}
