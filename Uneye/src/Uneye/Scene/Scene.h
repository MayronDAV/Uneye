#pragma once

#include <string>
#include <entt/entt.hpp>

#include "Uneye/Core/Timestep.h"

#include "Uneye/Renderer/EditorCamera.h"
#include "Uneye/Utils/PlatformUtils.h"



namespace Uneye
{
	class Entity;
	class SceneHierarchyPanel;
	class SceneSerializer;

	class Scene
	{
		public:
			Scene();
			~Scene();
			
			Entity CreateEntity(const std::string& name = std::string());
			void DestroyEntity(Entity entity);

			void OnUpdateEditor(Timestep ts, EditorCamera& camera);
			void OnUpdateRuntime(Timestep ts);
			void OnViewportResize(uint32_t width, uint32_t height);

			Entity GetPrimaryCameraEntity();

			float GetFPS() { return m_FPS; }

		private:
			template<typename T>
			void OnComponentAdded(Entity entity, T& component);

		private:
			entt::registry m_Registry;
			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

			float m_LastTime = Time::GetTime();
			float m_FPSCounter = 0;
			float m_FPS = 0;

			friend class Entity;
			friend class SceneHierarchyPanel;
			friend class SceneSerializer;
	};


}
