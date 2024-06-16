#pragma once

#include "Uneye/Asset/Asset.h"
#include "Uneye/Core/Timestep.h"
#include "Uneye/Core/UUID.h"
#include "Uneye/Renderer/EditorCamera.h"
#include "Uneye/Utils/PlatformUtils.h"
#include "Uneye/Core/TraitHelpers.h"

#include <string>
#include <entt/entt.hpp>





class b2World;

namespace Uneye
{
	class Entity;
	class SceneHierarchyPanel;
	class SceneSerializer;
	class RayPicking;

	class Scene : public Asset
	{
		public:
			Scene();
			Scene(const Scene&) = default;
			~Scene();
			
			static Ref<Scene> Copy(Ref<Scene> other);

			static AssetType GetStaticType() { return AssetType::Scene; }
			virtual AssetType GetType() const { return GetStaticType(); }

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

			Entity DuplicateEntity(Entity entt);

			Entity FindFirstEntityByName(std::string_view name);
			Entity GetEntityByUUID(UUID uuid);


			Entity GetPrimaryCameraEntity();

			float GetFPS() { return m_FPS; }

			bool IsRunning() const { return m_IsRunning; }
			bool IsPaused() const { return m_IsPaused; }

			void SetPaused(bool paused) { m_IsPaused = paused; }

			void Step(int frames = 1);

			template<typename... Components>
			auto GetAllEntitiesWith()
			{
				return m_Registry.view<Components...>();
			}


		private:
			template<typename T>
			void OnComponentAdded(Entity entity, T& component);

			template<typename T, typename = void>
			void OnComponentAddedImpl(Entity entity, T& component);

			template<typename T>
			void OnComponentAddedImpl(Entity entity, T& component,
				typename std::enable_if<has_OnComponentAdded_in_scene<Scene, void(Entity, T&)>::value>::type* = nullptr);


			void OnPhysics2DStart();
			void OnPhysics2DStop();

			void RenderScene(EditorCamera& camera);

		private:
			entt::registry m_Registry;
			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
			bool m_IsRunning = false;
			bool m_IsPaused = false;
			int m_StepFrames = 0;

			float m_LastTime = Time::GetTime();
			float m_FPSCounter = 0;
			float m_FPS = 0;

			b2World* m_PhysicsWorld = nullptr;

			std::unordered_map<UUID, Entity> m_EntityMap;

			friend class Entity;
			friend class SceneHierarchyPanel;
			friend class SceneSerializer;
			friend class RayPicking;
	};


}
