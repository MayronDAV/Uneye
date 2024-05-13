#include "Panels/SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Uneye/Scene/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UI/UI.h"
#include "Uneye/Renderer/Renderer2D.h"
#include "Uneye/Utils/PlatformUtils.h"


namespace Uneye
{
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

		ImGui::End();


		ImGui::Begin("Properties");

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);

		}

		ImGui::End();



		ImGui::Begin("Stats");

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

					if (!m_SelectionContext.HasComponent<MaterialComponent>())
					{
						if (ImGui::MenuItem("Material"))
						{
							m_SelectionContext.AddComponent<MaterialComponent>();
							ImGui::CloseCurrentPopup();
						}
					}

					ImGui::EndPopup();
				}
			}


			DrawComponentUI<TransformComponent>(entt, "Transform", [&](auto& tc) {

				UI::DrawVec3Control("Translation", tc.Translation);
				glm::vec3 rotation = glm::degrees(tc.Rotation);
				UI::DrawVec3Control("Rotation", rotation);
				tc.Rotation = glm::radians(rotation);
				UI::DrawVec3Control("Scale", tc.Scale, 1.0f);

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


			DrawComponentUI<MaterialComponent>(entt, "Material", [&](auto& mc) {

				UI::DrawColorEdit4("Color ", mc.Color, 1.0f);

				UI::DrawClickableText("Texture Path", mc.TexturePath, [&]() {

						mc.TexturePath = " ";
						mc.Texture = nullptr;
						mc.IsSubTexture = false;
						mc.SubTexture = nullptr;

				}, [&]() {
				
					std::string filepath = FileDialogs::OpenFile("img files (*.png)|*.jpg|All files (*.*)|*.*");
					std::string path = mc.TexturePath;
					if (filepath.empty())
					{
						mc.TexturePath = path;
					}
					else
						mc.TexturePath = filepath;

					if (mc.TexturePath == "" || mc.TexturePath == " " || mc.TexturePath == std::string())
					{
						mc.Texture = nullptr;
						mc.IsSubTexture = false;
					}
					else
						mc.Texture = Texture2D::Create(mc.TexturePath);

				});
					

				UI::DrawCheckBox("Is SubTexture ", &mc.IsSubTexture);

				if (mc.IsSubTexture)
				{
					UI::DrawVec2Input("Global Tile Size", mc.TileSize, 1.0f);
					UI::DrawVec2Input("Tile Coord", mc.Coords);
					UI::DrawVec2Input("Sprite Multiple Size", mc.SpriteSize, 1.0f);

					mc.SubTexture = SubTexture2D::CreateFromTexture(mc.Texture, mc.Coords, mc.TileSize, mc.SpriteSize);
				}

			}, true);
		}

	}

}
