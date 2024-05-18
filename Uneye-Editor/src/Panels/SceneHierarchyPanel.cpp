#include "Panels/SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Uneye/Scene/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UI/UI.h"
#include "Uneye/Renderer/Renderer2D.h"
#include "Uneye/Utils/PlatformUtils.h"

#include <filesystem>



namespace Uneye
{
	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entt)
			{
				Entity entity{ entt, m_Context.get() };
				DrawEntityNode(entity);
			});

			// Right click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}

		}

		ImGui::End();


		ImGui::Begin("Properties");

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);

		}

		ImGui::End();



		ImGui::Begin("Stats");

		UI::DrawTextArgs("FPS on editor ", " %.3f", m_Context->GetFPS());
		ImGui::Spacing();
		auto stats = Uneye::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats");
		UI::DrawTextArgs("Draw Calls", " %d", stats.DrawCalls);
		UI::DrawTextArgs("Quad Count",	" %d", stats.QuadCount);
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
		{
			if (m_SelectionContext == entt)
				m_SelectionContext = {};
			else
				m_SelectionContext = entt;
		}

		// Right click on item
		bool enttDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				enttDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (enttDeleted)
		{
			if (m_SelectionContext == entt)
				m_SelectionContext = {};
			m_Context->DestroyEntity(entt);
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
				if (ImGui::Button("+", {lineHeight, lineHeight}))
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
					if (!m_SelectionContext.HasComponent<CameraComponent>())
					{
						if (ImGui::MenuItem("Camera"))
						{
							m_SelectionContext.AddComponent<CameraComponent>();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!m_SelectionContext.HasComponent<SpriteComponent>())
					{
						if (ImGui::MenuItem("Sprite"))
						{
							m_SelectionContext.AddComponent<SpriteComponent>();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!m_SelectionContext.HasComponent<CircleComponent>())
					{
						if (ImGui::MenuItem("Circle"))
						{
							m_SelectionContext.AddComponent<CircleComponent>();
							ImGui::CloseCurrentPopup();
						}
					}
					if (!m_SelectionContext.HasComponent<Rigidbody2DComponent>())
					{
						if (ImGui::MenuItem("Rigidbody 2D"))
						{
							m_SelectionContext.AddComponent<Rigidbody2DComponent>();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
					{
						if (ImGui::MenuItem("BoxCollider 2D"))
						{
							m_SelectionContext.AddComponent<BoxCollider2DComponent>();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!m_SelectionContext.HasComponent<CircleCollider2DComponent>())
					{
						if (ImGui::MenuItem("CircleCollider 2D"))
						{
							m_SelectionContext.AddComponent<CircleCollider2DComponent>();
							ImGui::CloseCurrentPopup();
						}
					}


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


			DrawComponentUI<SpriteComponent>(entt, "Sprite", [&](auto& mc) {

				UI::DrawColorEdit4("Color ", mc.Color, 1.0f);

				std::string filename = std::filesystem::path(mc.TexturePath).filename().string();
				UI::DrawClickableText("Texture Path", filename, [&]() {

						mc.TexturePath = " ";
						mc.Texture = nullptr;
						mc.IsSubTexture = false;
						mc.SubTexture = nullptr;

				}, [&]() {
				
					std::string filepath = FileDialogs::OpenFile("img files (*.png)|*.jpg|All files (*.*)|*.*");
					std::string path = mc.TexturePath;
					if (filepath.empty())
						mc.TexturePath = path;
					else
						mc.TexturePath = filepath;

					if (mc.TexturePath == "" || mc.TexturePath == " " || mc.TexturePath == std::string())
					{
						mc.Texture = nullptr;
						mc.IsSubTexture = false;
					}
					else	
						mc.Texture = Texture2D::Create(mc.TexturePath);

					filename = std::filesystem::path(mc.TexturePath).filename().string();

				}, [&]() {
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							mc.TexturePath = texturePath.string();
							filename = std::filesystem::path(mc.TexturePath).filename().string();
							mc.Texture = Texture2D::Create(texturePath.string());
						}
						ImGui::EndDragDropTarget();
					}

				});
					
				UI::DrawCheckBox("Is SubTexture ", &mc.IsSubTexture);

				if (mc.IsSubTexture)
				{
					UI::DrawFloat2Input("Global Tile Size", mc.TileSize, 1.0f);
					UI::DrawFloat2Input("Tile Coord", mc.Coords);
					UI::DrawFloat2Input("Sprite Multiple Size", mc.SpriteSize, 1.0f);

					if (!(mc.TexturePath == "" || mc.TexturePath == " " || mc.TexturePath == std::string()))
						mc.SubTexture = SubTexture2D::CreateFromTexture(mc.Texture, mc.Coords, mc.TileSize, mc.SpriteSize);
					else
						mc.SubTexture = nullptr;
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
		}

	}

}
