#include "Panels/SceneHierarchyPanel.h"

#include "Uneye/Scene/Components.h"
#include "UI/UI.h"
#include "Uneye/Renderer/Renderer2D.h"
#include "Uneye/Utils/PlatformUtils.h"

#include "Uneye/Scripting/ScriptEngine.h"

#include "Uneye/Project/Project.h"
#include "Uneye/Asset/TextureImporter.h"
#include "Uneye/Asset/AssetManager.h"
#include "Uneye/Scene/SceneManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <filesystem>



namespace Uneye
{
	SceneHierarchyPanel::SceneHierarchyPanel(const ScenesMap& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const ScenesMap& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}
	
	void SceneHierarchyPanel::DrawSceneHierarchy(const std::filesystem::path& p_path, const Ref<Scene>& p_scene)
	{
		auto filename = p_path.stem().generic_string();

		ImGuiTreeNodeFlags flags = ((m_SelectedScene == p_scene) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow;

		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		if (m_OpenedDueToNewEntity && (m_SelectedScene == p_scene)) {
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::SetNextItemOpen(true);
			m_OpenedDueToNewEntity = false;
		}

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)p_scene->Handle, flags, filename.c_str());

		if (ImGui::IsItemClicked())
			m_SelectedScene = (m_SelectedScene == p_scene) ? nullptr : p_scene;

		// Right click on item
		bool sceneUnloaded = false;
		if (ImGui::BeginPopupContextItem())
		{
			m_SelectedScene = p_scene;

			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_OpenedDueToNewEntity = true;
				p_scene->CreateEntity("Empty Entity");
			}

			if (ImGui::MenuItem("Save Scene"))
			{
				//UNEYE_CORE_ERROR("Not Implemented for {}", filename);
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			auto idView = p_scene->m_Registry.view<TagComponent>();
			for (auto e : idView)
			{
				Entity entity{ e, p_scene.get() };
				DrawEntityNode(entity);
			}

			ImGui::TreePop();
		}

		if (sceneUnloaded)
		{
			m_SelectedScene = nullptr;
		}
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (!m_Context.empty())
		{
			bool isopen = false;
			Ref<Scene> curScene = nullptr;

			for (const auto& [path, scene] : m_Context)
			{
				DrawSceneHierarchy(path, scene);
			}

		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Load scene..."))
			{
				std::string filepath = FileDialogs::OpenFile("Uneye Scene (*.uyscene)\0*.uyscene\0");
				if (!filepath.empty())
				{
					SceneManager::LoadScene(filepath, LoadMode::Single);
				}
			}

			if (ImGui::MenuItem("Add new scene..."))
			{
				std::string filepath = FileDialogs::OpenFile("Uneye Scene (*.uyscene)\0*.uyscene\0");
				if (!filepath.empty())
				{
					SceneManager::LoadScene(filepath, LoadMode::Additive);
				}
			}

			ImGui::EndPopup();
		}

		ImGui::End();


		ImGui::Begin("Properties");

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);

		}

		ImGui::End();



		ImGui::Begin("Stats");

		double sum = 0;
		int count = m_Context.size();
		for (const auto& [path, scene] : m_Context)
		{
			sum += scene->GetFPS();
		}
		double fps = sum / count;

		UI::DrawTextArgs("FPS on editor ", " %.3f", fps);
		ImGui::Spacing();
		auto stats = Uneye::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats");
		UI::DrawTextArgs("Draw Calls", " %d", stats.DrawCalls);
		UI::DrawTextArgs("Quad Count", " %d", stats.QuadCount);
		UI::DrawTextArgs("Vertices", " %d", stats.GetTotalVertexCount());
		UI::DrawTextArgs("Indices", " %d", stats.GetTotalIndexCount());

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entt)
	{
		auto& tag = entt.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entt) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow;

		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entt, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			m_SelectionContext = (m_SelectionContext == entt) ? Entity{} : entt;

		// Right click on item
		bool enttDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				m_SelectionContext = entt;
				enttDeleted = true;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (enttDeleted)
		{
			SceneManager::DestroyEntity(&m_SelectionContext);
			m_SelectionContext = {};		
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	template<typename Component, typename Func>
	inline void SceneHierarchyPanel::DrawComponentUI(Entity entt, const std::string& name, const Func& func, bool settings)
	{

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		if (entt.HasComponent<Component>())
		{
			auto& component = entt.GetComponent<Component>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Spacing();
			bool open = ImGui::TreeNodeEx((void*)typeid(Component).hash_code(),
				treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if (settings)
			{
				ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
				if (ImGui::Button("+", { lineHeight, lineHeight }))
					ImGui::OpenPopup("ComponentSettings");

				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component"))
						removeComponent = true;

					ImGui::EndPopup();
				}

			}

			if (open)
			{
				func(component);

				ImGui::TreePop();
			}


			if (removeComponent)
				entt.RemoveComponent<Component>();

		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entt)
	{
		if (entt)
		{
			if (entt.HasComponent<IDComponent>())
			{
				auto& id = entt.GetComponent<IDComponent>().ID;


				UI::DrawTextArgs("ID", " %llu", id);
			}

			if (entt.HasComponent<TagComponent>())
			{
				auto& tag = entt.GetComponent<TagComponent>().Tag;


				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), tag.c_str());
				if (UI::DrawInputText("Tag", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
			}

			if (m_SelectionContext)
			{
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);

				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("AddComponent");

				if (ImGui::BeginPopup("AddComponent"))
				{

					DisplayAddComponentEntry<CameraComponent>("Camera");
					DisplayAddComponentEntry<ScriptComponent>("C# Script");
					DisplayAddComponentEntry<SpriteComponent>("Sprite");
					DisplayAddComponentEntry<CircleComponent>("Circle");
					DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
					DisplayAddComponentEntry<BoxCollider2DComponent>("BoxCollider 2D");
					DisplayAddComponentEntry<CircleCollider2DComponent>("CircleCollider 2D");
					DisplayAddComponentEntry<TextComponent>("Text Component");

					ImGui::EndPopup();
				}
			}


			DrawComponentUI<TransformComponent>(entt, "Transform", [&](auto& tc) {

				UI::DrawFloat3Control("Translation", tc.Translation);
				glm::vec3 rotation = glm::degrees(tc.Rotation);
				UI::DrawFloat3Control("Rotation", rotation);
				tc.Rotation = glm::radians(rotation);
				UI::DrawFloat3Control("Scale", tc.Scale, 1.0f);

				});

			DrawComponentUI<CameraComponent>(entt, "Camera", [&](auto& cameraComponent) {

				auto& camera = cameraComponent.Camera;

				UI::DrawCheckBox("Primary", &cameraComponent.Primary);

				const char* projectionTypeString[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeString[
					(int)camera.GetProjectionType()];

				UI::DrawCombo("Projection", currentProjectionTypeString, [&]() {
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeString[i];
						if (ImGui::Selectable(projectionTypeString[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeString[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					});


				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					float perspNear = camera.GetPerspectiveNearClip();
					float perspFar = camera.GetPerspectiveFarClip();


					if (UI::DrawFloatControl("FOV", perspFOV))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspFOV));

					if (UI::DrawFloatControl("Near", perspNear))
						camera.SetPerspectiveNearClip(perspNear);

					if (UI::DrawFloatControl("Far", perspFar))
						camera.SetPerspectiveFarClip(perspFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					float orthoNear = camera.GetOrthographicNearClip();
					float orthoFar = camera.GetOrthographicFarClip();


					if (UI::DrawFloatControl("Size", orthoSize))
						camera.SetOrthographicSize(orthoSize);

					if (UI::DrawFloatControl("Near", orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					if (UI::DrawFloatControl("Far", orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					UI::DrawCheckBox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
				}

				}, true);

			DrawComponentUI<ScriptComponent>(entt, "Script", [&](auto& p_sc)
			{
				bool scriptClassExists = ScriptEngine::EntitySubClassExists(p_sc.Name);

				char buffer[64];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), p_sc.Name.c_str());
				if (UI::DrawInputText("Class", buffer, sizeof(buffer)))
				{
					p_sc.Name = std::string(buffer);
				}

				bool sceneRunning = (SceneManager::GetState() == SceneState::Play);
				if (sceneRunning)
				{
					Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entt.GetUUID());
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();
						for (const auto& [name, field] : fields)
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptInstance->GetFieldValue<float>(name);
								if (UI::DrawFloatControl(name, data))
									scriptInstance->SetFieldValue(name, data);
							}
						}
					}
				}
				else
				{
					if (scriptClassExists)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(p_sc.Name);
						const auto& fields = entityClass->GetFields();

						auto& entityFields = ScriptEngine::GetScriptFieldMap(entt);
						for (const auto& [name, field] : fields)
						{
							// Field has been set in editor
							if (entityFields.find(name) != entityFields.end())
							{
								ScriptFieldInstance& scriptField = entityFields.at(name);

								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = scriptField.GetValue<float>();
									if (UI::DrawFloatControl(name, data))
										scriptField.SetValue(data);
								}
							}
							else
							{
								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = 0.0f;
									if (UI::DrawFloatControl(name, data))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue(data);
									}
								}
							}
						}
					}
				}


			}, true);

			DrawComponentUI<SpriteComponent>(entt, "Sprite", [&](auto& p_sc) {

				UI::DrawColorEdit4("Color ", p_sc.Color, 1.0f);

				std::string filename = std::filesystem::path(p_sc.TexturePath).filename().string();
				UI::DrawClickableText("Texture Path", filename, [&]() {

					p_sc.TexturePath = "";
					p_sc.IsSubTexture = false;
					p_sc.Texture = 0;

					}, [&]() {

						std::string filepath = FileDialogs::OpenFile("All files (*.*) | *.*");
						std::string path = p_sc.TexturePath;
						if (filepath.empty())
							p_sc.TexturePath = path;
						else
							p_sc.TexturePath = filepath;

						if (!p_sc.TexturePath.empty())
							p_sc.Texture = AssetManager::ImportAsset(p_sc.TexturePath);

						filename = std::filesystem::path(p_sc.TexturePath).filename().string();

					}, [&]() {
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
							{
								const wchar_t* path = (const wchar_t*)payload->Data;
								std::filesystem::path texturePath = Project::GetActiveAssetFileSystemPath(path);
								p_sc.TexturePath = texturePath.string();
								p_sc.Texture = AssetManager::ImportAsset(p_sc.TexturePath);
							}
							ImGui::EndDragDropTarget();
						}
				});

				UI::DrawCheckBox("Is SubTexture ", &p_sc.IsSubTexture);

				if (p_sc.IsSubTexture)
				{
					UI::DrawFloat2Input("Tile Size", p_sc.TileSize, 1.0f);
					UI::DrawFloat2Input("Tile Coord", p_sc.TileCoord);
					UI::DrawFloat2Input("Sprite Size", p_sc.SpriteSize, 1.0f);
				}

				}, true);


			DrawComponentUI<CircleComponent>(entt, "Circle", [&](auto& cc) {

				UI::DrawColorEdit4("Color ", cc.Color, 1.0f);

				//UI::DrawFloatControl("Radius", cc.Radius);
				UI::DrawFloatControl("Thickness", cc.Thickness, 0.025f, 0.1f, 1.0f);
				UI::DrawFloatControl("Fade", cc.Fade, 0.00025f, 0.00f, 1.0f);

				}, true);

			DrawComponentUI<Rigidbody2DComponent>(entt, "Rigidbody 2D", [&](auto& rb2d) {

				const char* rigidbodyTypeString[] = { "Static", "Dynamic" };
				const char* currentrigidbodyTypeString = rigidbodyTypeString[
					(int)rb2d.Type];

				UI::DrawCombo("Type", currentrigidbodyTypeString, [&]() {
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentrigidbodyTypeString == rigidbodyTypeString[i];
						if (ImGui::Selectable(rigidbodyTypeString[i], isSelected))
						{
							currentrigidbodyTypeString = rigidbodyTypeString[i];
							rb2d.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					});

				UI::DrawCheckBox("FixedRotation", &rb2d.FixedRotation);

				}, true);


			DrawComponentUI<BoxCollider2DComponent>(entt, "BoxCollider 2D", [&](auto& bc2d) {

				UI::DrawFloat2Control("Size", bc2d.Size);
				UI::DrawFloat2Control("Offset", bc2d.Offset);

				UI::DrawFloatControl("Density", bc2d.Density);
				UI::DrawFloatControl("Friction", bc2d.Friction);
				UI::DrawFloatControl("Restitution", bc2d.Restitution, 0.0f, 0.0f, 1.0f);
				UI::DrawFloatControl("RestitutionThreshold", bc2d.RestitutionThreshold, 0.0f, 0.0f, 1.0f);

				}, true);

			DrawComponentUI<CircleCollider2DComponent>(entt, "CircleCollider 2D", [&](auto& cc2d) {

				UI::DrawFloat2Control("Offset", cc2d.Offset);
				UI::DrawFloatControl("Radius", cc2d.Radius);

				UI::DrawFloatControl("Density", cc2d.Density);
				UI::DrawFloatControl("Friction", cc2d.Friction);
				UI::DrawFloatControl("Restitution", cc2d.Restitution, 0.0f, 0.0f, 1.0f);
				UI::DrawFloatControl("RestitutionThreshold", cc2d.RestitutionThreshold, 0.0f, 0.0f, 1.0f);

				}, true);

			DrawComponentUI<TextComponent>(entt, "Text Renderer", [](auto& component)
			{
					ImGui::InputTextMultiline("Text String", &component.TextString);
					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
					ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
					ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);

			}, true);
		}

	}
}
