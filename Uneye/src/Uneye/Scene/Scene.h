#pragma once

#include "Uneye/Core/Timestep.h"
#include "Uneye/Core/UUID.h"
#include "Uneye/Renderer/EditorCamera.h"
#include "Uneye/Utils/PlatformUtils.h"

#include <string>
#include <entt/entt.hpp>




class b2World;

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
			
			static Ref<Scene> Copy(Ref<Scene> other);

			Entity CreateEntity(const std::string& name = std::string());
			Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
			void DestroyEntity(Entity entity);

			void OnRuntimeStart();
			void OnRuntimeStop();

			void OnSimulationStart();
			void OnSimulationStop();

			void OnUpdateEditor(Timestep ts, EditorCamera& camera);
			void OnUpdateRuntime(Timestep ts);
			void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
			void OnViewportResize(uint32_t width, uint32_t height);

			void DuplicateEntity(Entity entt);

			Entity GetPrimaryCameraEntity();

			float GetFPS() { return m_FPS; }

			template<typename... Components>
			auto GetAllEntitiesWith()
			{
				return m_Registry.view<Components...>();
			}


		private:
			template<typename T>
			void OnComponentAdded(Entity entity, T& component);

			void OnPhysics2DStart();
			void OnPhysics2DStop();
		private:
			entt::registry m_Registry;
			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

			float m_LastTime = Time::GetTime();
			float m_FPSCounter = 0;
			float m_FPS = 0;

			b2World* m_PhysicsWorld = nullptr;

			friend class Entity;
			friend class SceneHierarchyPanel;
			friend class SceneSerializer;
	};


}
