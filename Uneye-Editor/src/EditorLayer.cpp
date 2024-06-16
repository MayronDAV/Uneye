#include "EditorLayer.h"


#include "Uneye/Scene/SceneSerializer.h"
#include "Uneye/Utils/PlatformUtils.h"
#include "Uneye/Scripting/ScriptEngine.h"
#include "Uneye/Renderer/Font.h"

#include "Uneye/Core/ThemeManager.h"

#include "Uneye/Math/Math.h"

#include "Uneye/Asset/AssetManager.h"
#include "Uneye/Asset/TextureImporter.h"
#include "Uneye/Asset/SceneImporter.h"

#include "Uneye/Scene/SceneManager.h"

#include <imgui/imgui.h>
#include <ImGuizmo.h>




namespace Uneye
{
	static Ref<Font> s_Font;

	EditorLayer::EditorLayer()
		:Layer("EditorLayer")
	{
		s_Font = Font::GetDefault();
	}

	void EditorLayer::OnAttach()
	{
		UNEYE_PROFILE_FUNCTION();


		//Application::Get().GetWindow().SetVSync(false);

		m_IconPlay = TextureImporter::LoadTexture2D("Resources/Icons/PlayButton.png");
		m_IconPause = TextureImporter::LoadTexture2D("Resources/Icons/PauseButton.png");
		m_IconSimulate = TextureImporter::LoadTexture2D("Resources/Icons/SimulateButton.png");
		m_IconStep = TextureImporter::LoadTexture2D("Resources/Icons/StepButton.png");
		m_IconStop = TextureImporter::LoadTexture2D("Resources/Icons/StopButton.png");

		FramebufferSpecification fbspec;
		fbspec.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::R32I,
			FramebufferTextureFormat::RG32UI,
			FramebufferTextureFormat::Depth,
		};
		fbspec.Width = 800;
		fbspec.Height = 600;
		m_Framebuffer = Framebuffer::Create(fbspec);

		SceneManager::Init();

		// Move this to another place
		bool useCommandLineArgs = false;
		if (useCommandLineArgs)
		{
			auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
			if (commandLineArgs.Count > 1)
			{
				auto projectFilePath = commandLineArgs[1];
				OpenProject(projectFilePath);
			}
			else
			{
				// TODO: create a way to the user select or create a new project
				NewProject();
			}
		}
		else
		{
			//OpenProject("SandboxProject/Sandbox.uyproj");
			if (!OpenProject())
				Application::Get().Close();
		}

		std::filesystem::path path = (Project::GetActiveAssetDirectory() / "Scenes/PinkCubeContainer.uyscene");
		SceneManager::LoadScene(path.string(), LoadMode::Additive);

		Renderer2D::SetLineWidth(4.0f);


		m_SceneHierarchyPanel.SetContext(SceneManager::GetScenes());

		m_RayPicking.Init();
	}

	void EditorLayer::OnDetach()
	{
		UNEYE_PROFILE_FUNCTION();

		SceneManager::Shutdown();
	}

	bool first = true;
	void EditorLayer::OnUpdate(Timestep ts)
	{
		UNEYE_PROFILE_FUNCTION();

		if (first)
		{
			// TODO:
			m_SceneHierarchyPanel.SetContext(SceneManager::GetScenes());
			first = false;
		}


		SceneManager::Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		if (Uneye::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		//RenderCommand::Clear(glm::vec4(1.0f));
		RenderCommand::ClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		RenderCommand::ClearDepth();

		int value = -1;
		m_Framebuffer->ClearAttachment(1, &value);
		std::vector<uint32_t> uvalue = { 0, 0 };
		m_Framebuffer->ClearAttachment(2, uvalue.data());

		SceneManager::OnUpdate(ts);

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
			m_RayPicking.SetTarget(glm::vec3(mouseX, mouseY, 0.0f));
		}
		m_RayPicking.SetFocus(m_ViewportHovered);
		m_RayPicking.SetFramebuffer(m_Framebuffer);
		m_HoveredEntity = m_RayPicking.GetHoveredEntity();

		OnOverlayRender();

		m_RayPicking.OnUpdate();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		SceneManager::OnEditorCameraEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(UNEYE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(UNEYE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<WindowDropEvent>(UNEYE_BIND_EVENT_FN(EditorLayer::OnWindowDrop));
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
				if (ImGui::MenuItem("Save Project..."))
					SaveProject();

				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					OpenProject();

				ImGui::Separator();

				if (ImGui::MenuItem("New", "Ctrl+N"))
					SceneManager::NewScene();

				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SceneManager::SaveScene();

				//if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					//SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{

				if (ImGui::MenuItem("Set Default Scene"))
					Project::SetStartScene(AssetManager::ImportAsset(SceneManager::GetCurrentScenePath()));

				ImGui::Separator();

				if (ImGui::MenuItem("Import..."))
					m_AssetImporterPanelIsOpen = true;

				ImGui::Separator();

				if (ImGui::MenuItem("Reload ImGui"))
					Application::Get().ReloadImGui();

				ImGui::Separator();

				if (ImGui::BeginMenu("Themes"))
				{
					for (const auto& theme : ThemeManager::GetThemes())
					{
						const auto& name = theme.first;
						if (ImGui::MenuItem(name.c_str()))
							Application::Get().ChangeTheme(name);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("Reload Assembly", "Ctrl+R"))
					ReloadAssembly();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_AssetRegistryPanel.OnImGuiRender();
		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();
		m_LogPanel.OnImGuiRender();

		ImGui::Begin("Settings");
		ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);

		ImGui::Image((ImTextureID)s_Font->GetAtlasTexture()->GetRendererID(), { 512,512 }, { 0, 1 }, { 1, 0 });
		ImGui::End();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };		m_ViewportFocused = ImGui::IsWindowFocused();
		
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportSize.x, viewportSize.y };
		uint32_t texID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)texID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path filepath = Project::GetActiveAssetFileSystemPath(path);
				if (filepath.extension() == ".uyscene") SceneManager::LoadScene(filepath.string());
			}
			ImGui::EndDragDropTarget();
		}

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
			auto editorCamera = SceneManager::GetEditorCamera();
			const glm::mat4& cameraProjection = editorCamera.GetProjection();
			glm::mat4 cameraView = editorCamera.GetViewMatrix();


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

		UI_Toolbar();

		if (m_AssetImporterPanelIsOpen)
		{
			m_AssetImporterPanel.SetOpen();
			m_AssetImporterPanel.Open();
		}

		if (!m_AssetImporterPanel)
			m_AssetImporterPanelIsOpen = false;

		ImGui::End();

		//ImGui::ShowDemoWindow();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = !SceneManager::GetScenes().empty();

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		auto sceneState = SceneManager::GetState();
		bool hasPlayButton = sceneState == SceneState::Edit || sceneState == SceneState::Play;
		bool hasSimulateButton = sceneState == SceneState::Edit || sceneState == SceneState::Simulate;
		bool hasPauseButton = sceneState != SceneState::Edit;

		if (hasPlayButton)
		{
			Ref<Texture2D> icon = (sceneState == SceneState::Edit || sceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				SceneManager::Play();
			}
		}

		if (hasSimulateButton)
		{
			if (hasPlayButton)
				ImGui::SameLine();

			Ref<Texture2D> icon = (sceneState == SceneState::Edit || sceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				SceneManager::Simulate();
			}
		}
		if (hasPauseButton)
		{
			bool isPaused = SceneManager::IsPaused();
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = m_IconPause;
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					isPaused = !isPaused;
					SceneManager::Pause(isPaused);
				}
			}

			// Step button
			if (isPaused)
			{
				ImGui::SameLine();
				Ref<Texture2D> icon = m_IconStep;
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					SceneManager::Step(60);
				}
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					SceneManager::NewScene();

				break;
			}
			case Key::O:
			{
				if (control)
					OpenProject();

				break;
			}
			case Key::S:
			{
				if (control)
					SceneManager::SaveScene();

				break;
			}

			// Scene Commands
			case Key::D:
			{
				if (control)
					OnDuplicateEntity();

				break;
			}


			case Key::Delete:
			{
				OnDestroyEntity();

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
				if (control)
				{
					if (SceneManager::GetState() != SceneState::Edit)
						SceneManager::Stop();

					ScriptEngine::ReloadAssembly();
				}
				break;

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

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::Button_Left)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
		}
		return false;
	}

	bool EditorLayer::OnWindowDrop(WindowDropEvent& e)
	{
		// TODO: if a project is dropped in, probably open it
		
		//AssetManager::ImportAsset();

		return true;
	}

	void EditorLayer::OnOverlayRender()
	{
		for (const auto& [path, scene] : SceneManager::GetScenes())
		{
			if (SceneManager::GetState() == SceneState::Play)
			{
				Entity camera = scene->GetPrimaryCameraEntity();
				if (!camera)
					return;

				Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
			}
			else
			{
				Renderer2D::BeginScene(SceneManager::GetEditorCamera());
			}

			if (m_ShowPhysicsColliders)
			{

				{
					// Box Colliders
					{
						auto view = scene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
						for (auto entity : view)
						{
							auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

							glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
							glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

							glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
								* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
								* glm::scale(glm::mat4(1.0f), scale);

							Renderer2D::DrawRect(transform, glm::vec4(0, 0, 1, 1));
						}
					}

					// Circle Colliders
					{
						auto view = scene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
						for (auto entity : view)
						{
							auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

							glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
							glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

							glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
								* glm::scale(glm::mat4(1.0f), scale);

							Renderer2D::DrawCircle(transform, glm::vec4(0, 0, 1, 1), 0.01f);
						}
					}
				}
			}

			if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity()) {
				TransformComponent transform = selectedEntity.GetComponent<TransformComponent>();

				Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
			}


			Renderer2D::EndScene();
		}

	}

	void EditorLayer::ReloadAssembly()
	{
		if (SceneManager::GetState() != SceneState::Edit)
		{
			UNEYE_CORE_ERROR("Scene is running!!!");
			UNEYE_CORE_WARN("Stopping the scene... Remember to stop the scene before reloading assembly");
			SceneManager::Stop();
		}

		// TODO: a way to reload assembly while scene is running.
		ScriptEngine::ReloadAssembly();
	}

	void EditorLayer::NewProject()
	{
		Project::New();
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Uneye Project (*.uyproj)\0*.uyproj\0");
		if (filepath.empty())
			return false;


		OpenProject(filepath);
		return true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (path.extension() == ".uyproj" && Project::Load(path))
		{
			ScriptEngine::Init();

			AssetHandle startScene = Project::GetActive()->GetConfig().StartScene;
			if (startScene)
				SceneManager::LoadScene(startScene);
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>(Project::GetActive());
		}
	}

	void EditorLayer::SaveProject()
	{
		Project::SaveActive(Project::GetActiveProjectFile());
	}

#if 0
	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_EditScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("Uneye Scene (*.uyscene)\0*.uyscene\0");
		std::filesystem::path filepath = std::filesystem::path(path);
		if (!filepath.empty())
		{
			OpenScene(AssetManager::ImportAsset(filepath));
		}
	}

	void EditorLayer::OpenScene(AssetHandle handle)
	{
		UNEYE_CORE_ASSERT(!handle);

		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		Ref<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
		if (!readOnlyScene)
		{
			UNEYE_CORE_CRITICAL("Do you have a registry for this scene?");
			return;
		}

		Ref<Scene> newScene = Scene::Copy(readOnlyScene);

		m_EditorScene = newScene;
		m_SceneHierarchyPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;
		m_EditScenePath = Project::GetActive()->GetEditorAssetManager()->GetFilePath(handle);
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Uneye Scene (*.uyscene)\0*.uyscene\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);

			m_EditScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneImporter::SaveScene(scene, path);
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(true);


	}

	void EditorLayer::OnSceneStop()
	{
		UNEYE_ASSERT(m_SceneState != SceneState::Play && m_SceneState != SceneState::Simulate, "Unknown state!");

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}
#endif

	void EditorLayer::OnDuplicateEntity()
	{
		if (SceneManager::GetState() != SceneState::Edit)
			return;

		Entity selectedEntt = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntt)
		{
			Entity newEntity = SceneManager::GetEditorScene()->DuplicateEntity(selectedEntt);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}

	void EditorLayer::OnDestroyEntity()
	{
		if (SceneManager::GetState() != SceneState::Edit)
			return;

		Entity selectedEntt = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntt)
		{
			m_SceneHierarchyPanel.SetSelectedEntity({});
			SceneManager::GetEditorScene()->DestroyEntity(selectedEntt);
		}
	}

}
