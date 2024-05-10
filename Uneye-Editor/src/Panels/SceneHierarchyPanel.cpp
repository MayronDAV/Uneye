#include "Panels/SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Uneye/Scene/Components.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UI/UI.h"
#include "Uneye/Renderer/Renderer2D.h"


namespace Uneye
{



	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.each([&](auto entt)
		{
			Entity entity{ entt, m_Context.get() };
			DrawEntityNode(entity);
		});

		ImGui::End();


		ImGui::Begin("Properties");

		DrawComponents(m_SelectionContext);

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

	void SceneHierarchyPanel::DrawEntityNode(Entity entt)
	{
		auto& tag = entt.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entt) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entt, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			if (m_SelectionContext == entt)
				m_SelectionContext = {};
			else
				m_SelectionContext = entt;
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}

	template<typename Component, typename Func>
	inline void SceneHierarchyPanel::DrawComponentUI(Entity entt, const std::string& name, const Func& func)
	{
		if (entt.HasComponent<Component>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(Component).hash_code(),
				ImGuiTreeNodeFlags_DefaultOpen, name.c_str()))
			{
				func();

				ImGui::TreePop();
			}
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

			DrawComponentUI<TransformComponent>(entt, "Transform", [&]() {

				auto& tc = entt.GetComponent<TransformComponent>();
				UI::DrawVec3Control("Translation", tc.Translation);
				glm::vec3 rotation = glm::degrees(tc.Rotation);
				UI::DrawVec3Control("Rotation", rotation);
				tc.Rotation = glm::radians(rotation);
				UI::DrawVec3Control("Scale", tc.Scale, 1.0f);

			});
			
			DrawComponentUI<CameraComponent>(entt, "Camera", [&]() {

				auto& cameraComponent = entt.GetComponent<CameraComponent>();
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

			});


			DrawComponentUI<SpriteRendererComponent>(entt, "Sprite Renderer", [&]() {

				auto& src = entt.GetComponent<SpriteRendererComponent>();
				UI::DrawColorEdit4("Color", src.Color);

			});

		}

	}

}