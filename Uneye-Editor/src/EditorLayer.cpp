#include "EditorLayer.h"

#include <imgui/imgui.h>

#include "Uneye/Scene/SceneSerializer.h"
#include "Uneye/Utils/PlatformUtils.h"


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

		//auto square = m_ActiveScene->CreateEntity("Color Square");
		//square.AddComponent<MaterialComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });
		//square.GetComponent<TransformComponent>().Translation = glm::vec3(6, 0, 0);
		//m_SquareEntity = square;

		int tileWidth = 10, tileHeight = 10;
		for (int y = 0; y < tileHeight; y++)
		{
			for (int x = 0; x < tileWidth; x++)
			{
				auto& square = m_ActiveScene->CreateEntity(
					"Tile Square " + std::to_string(x + (y * tileWidth)));

				square.GetComponent<TransformComponent>().Translation = glm::vec3(x * 0.6f, y * 0.6f, 0);
				square.GetComponent<TransformComponent>().Scale = glm::vec3(0.5f, 0.5f, 1.0f);

				square.AddComponent<MaterialComponent>(glm::vec4(0.3f, 0.8f, 0.2f, 1.0f));
			}
		}
		
		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
		m_SecondCamera.AddComponent<CameraComponent>();
		m_SecondCamera.GetComponent<CameraComponent>().Primary = false;

		class CameraController : public ScriptableEntity
		{
			public:
				void OnCreate()
				{
					auto& translation = GetComponent<TransformComponent>().Translation;
					translation.x = rand() % 10 - 5.0f;
				}

				void OnDestroy()
				{

				}

				void OnUpdate(Timestep ts)
				{
					//UNEYE_INFO("Timestep: {0}", ts.GetSeconds());
					auto& translation = GetComponent<TransformComponent>().Translation;
					m_Direction = glm::vec3(0.0f);

					if (Input::IsKeyPressed(Key::W))
						m_Direction.y =  1.0f;
					if (Input::IsKeyPressed(Key::A))
						m_Direction.x = -1.0f;
					if (Input::IsKeyPressed(Key::S))
						m_Direction.y = -1.0f;
					if (Input::IsKeyPressed(Key::D))
						m_Direction.x =  1.0f;

					m_Direction = m_Direction / ((glm::length(m_Direction) != 0) ?
						glm::length(m_Direction) : 1.0f);

					translation += m_Direction * m_Velocity * ts.GetSeconds();

				}

			private:
				glm::vec3 m_Direction{ 0.0f };
				glm::vec3 m_Velocity{ 10.0f };
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();


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

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(UNEYE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
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
		}
	}
	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::optional<std::string> filepath = FileDialogs::OpenFile("Uneye Scene (*.uneye)\0*.uneye\0");
		if (filepath != std::nullopt)
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(*filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::optional<std::string>  filepath = FileDialogs::SaveFile("Uneye Scene (*.uneye)\0*.uneye\0");
		if (filepath != std::nullopt)
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(*filepath);
		}
	}
}
