#include "uypch.h"
#include "RayPicking.h"

#include <Uneye/Scene/Scene.h>
#include "Uneye/Core/Input.h"
#include "Uneye/Scene/SceneManager.h"
#include "Uneye/Asset/AssetManager.h"

#include <imgui.h>




namespace Uneye
{

	RayPicking::RayPicking()
		: m_Target(0.0f), m_Direction(0, 0, -1)
	{
	}

	RayPicking::RayPicking(const glm::vec3& p_target)
		: m_Target(p_target), m_Direction(0, 0, -1)
	{
	}

	RayPicking::~RayPicking()
	{
	}

	void RayPicking::Init()
	{
		if (!m_UseSpecifiedScenes)
		{
			m_Scenes.clear();
			for (auto& [path, scene] : SceneManager::GetScenes())
			{
				m_Scenes.push_back(scene);
			}
		}

		if (!m_UseSpecifiedTarget)
		{
			m_Target = { Input::GetMouseX(), Input::GetMouseY(), 0.0f };
			//ScreenToWorld();
		}
	}

	void RayPicking::SetScenes(const Ref<Scene>& p_scene)
	{
		if (!m_UseSpecifiedScenes)
			m_Scenes.clear();

		m_UseSpecifiedScenes = true;

		auto it = std::find(m_Scenes.begin(), m_Scenes.end(), p_scene);
		if (it == m_Scenes.end())
			m_Scenes.push_back(p_scene);
	}

	void RayPicking::UnsetScene(const Ref<Scene>& p_scene)
	{
		if (m_Scenes.empty())
		{
			UNEYE_CORE_ERROR("Call this after setting scenes or Init func.");
			return;
		}

		auto it = std::find(m_Scenes.begin(), m_Scenes.end(), p_scene);
		if (it == m_Scenes.end())
			m_Scenes.erase(it);
	}

	void RayPicking::SetTarget(const glm::vec3& p_target)
	{
		m_UseSpecifiedTarget = true;

		m_Target = p_target;
	}

	void RayPicking::SetDirection(const glm::vec3& p_direction)
	{
		m_Direction = p_direction;
	}

	void RayPicking::SetFocus(bool p_focus)
	{
		m_IsFocused = p_focus;
	}

	void RayPicking::SetFramebuffer(Ref<Framebuffer>& framebuffer)
	{
		m_Framebuffer = framebuffer;
	}

	Entity RayPicking::GetHoveredEntity()
	{
		return m_HoveredEntity;
	}

	void RayPicking::OnUpdate()
	{

		if (!m_IsFocused)
		{
			//UNEYE_CORE_WARN("window unfocused");
			return;
		}

		if (m_Framebuffer == nullptr)
		{
			UNEYE_CORE_CRITICAL("You didn't call SetFramebuffer");
			return;
		}

		if (!m_UseSpecifiedTarget)
		{
			auto [x, y] = Input::GetMousePosition();

			m_Target = glm::vec3(x, y, 0.0f);
		}

		// Vincula o framebuffer que contém a textura
		m_Framebuffer->Bind();

		int enttID;
		m_Framebuffer->ReadPixel(1, m_Target.x, m_Target.y, 1, 1, &enttID);

		std::vector<uint32_t> pixel(2);
		m_Framebuffer->ReadPixel(2, m_Target.x, m_Target.y, 1, 1, pixel.data());

		m_Framebuffer->Unbind();

		uint64_t sceneHandle = (static_cast<uint64_t>(pixel[0]) << 32) | pixel[1];
		if (sceneHandle != 0)
		{
			auto scene = AssetManager::GetAsset<Scene>(sceneHandle);
			m_HoveredEntity = Entity((entt::entity)enttID, scene.get());
		}
		else
		{
			m_HoveredEntity = {};
		}

		//UNEYE_CORE_WARN("Mouse Pos: ({}, {})", m_Target.x, m_Target.y);
		//UNEYE_CORE_INFO("Entity ID: {}", enttID);
		//UNEYE_CORE_INFO("Scene Handle: ({}, {})", pixel[0], pixel[1]);
		//UNEYE_CORE_INFO("Combined Scene Handle: {}", sceneHandle);
	}

}
