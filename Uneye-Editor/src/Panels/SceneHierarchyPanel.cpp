#include "Panels/SceneHierarchyPanel.h"

#include <imgui/imgui.h>

#include "Uneye/Scene/Components.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


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
				if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
			}

			DrawComponentUI<TransformComponent>(entt, "Transform", [&]() {

				auto& transform = entt.GetComponent<TransformComponent>().Transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

			});
			
			DrawComponentUI<CameraComponent>(entt, "Camera", [&]() {

				auto& cameraComponent = entt.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				ImGui::Checkbox("Primary", &cameraComponent.Primary);

				const char* projectionTypeString[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeString[
					(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
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

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					float perspNear = camera.GetPerspectiveNearClip();
					float perspFar = camera.GetPerspectiveFarClip();


					if (ImGui::DragFloat("FOV", &perspFOV))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspFOV));

					if (ImGui::DragFloat("Near", &perspNear))
						camera.SetPerspectiveNearClip(perspNear);

					if (ImGui::DragFloat("Far", &perspFar))
						camera.SetPerspectiveFarClip(perspFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					float orthoNear = camera.GetOrthographicNearClip();
					float orthoFar = camera.GetOrthographicFarClip();


					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
				}

			});


			DrawComponentUI<SpriteRendererComponent>(entt, "Sprite Renderer", [&]() {

				auto& src = entt.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));

			});

		}

	}

}