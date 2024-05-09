#pragma once

#include <string>
#include <entt/entt.hpp>

#include "Uneye/Core/Timestep.h"




namespace Uneye
{
	class Entity;
	class SceneHierarchyPanel;

	class Scene
	{
		public:
			Scene();
			~Scene();
			
			Entity CreateEntity(const std::string& name = std::string());

			void OnUpdate(Timestep ts);
			void OnViewportResize(uint32_t width, uint32_t height);

		private:
			entt::registry m_Registry;
			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

			friend class Entity;
			friend class SceneHierarchyPanel;

	};


}