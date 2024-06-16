#pragma once
#include "Uneye/Scene/Entity.h"
#include "Uneye/Renderer/Framebuffer.h"

#include <glm/glm.hpp>
#include <cmath>



namespace Uneye
{
	class Entity;
	class Scene;

	class RayPicking
	{
		public:
			RayPicking();
			RayPicking(const glm::vec3& p_target);
			~RayPicking();

			void Init();

			// Call this for each scene that should be used,
			// otherwise it will be tested for all scenes currently in use.
			void SetScenes(const Ref<Scene>& p_scene);

			// Call this for each scene that should not be used.
			void UnsetScene(const Ref<Scene>& p_scene);

			// Don't call this if you want to picking with mouse.
			void SetTarget(const glm::vec3& p_target);
			void SetDirection(const glm::vec3& p_direction);
			void SetFocus(bool p_focus);

			void SetFramebuffer(Ref<Framebuffer>& framebuffer);

			Entity GetHoveredEntity();

			void OnUpdate();

		private:
			Entity m_HoveredEntity = {};


		private:
			bool m_UseSpecifiedTarget = false;
			bool m_UseSpecifiedScenes = false;
			bool m_IsFocused = false;
			bool m_NoZLimit = true;

			std::vector<Ref<Scene>> m_Scenes;
			glm::vec3 m_Target;
			glm::vec3 m_Direction;

			float m_ZLimit = -1.0f;
			glm::vec2 m_ViewportBounds[2];
			glm::vec2 m_ViewportSize;
			Ref<Framebuffer> m_Framebuffer = nullptr;
	};

}
