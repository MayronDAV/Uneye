#include "uypch.h"
#include "Uneye/Scene/Scene.h"

#include "Uneye/Scene/Components.h"
#include "Uneye/Scene/ScriptableEntity.h"
#include "Uneye/Renderer/Renderer2D.h"

#include "Uneye/Scene/Entity.h"
#include "Uneye/Core/Timer.h"

#include "Uneye/Scripting/ScriptEngine.h"

#include "Uneye/Physics/Physics2D.h"

#include <glm/glm.hpp>
#include <chrono>

#include "Uneye/Scene/SceneManager.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"



namespace Uneye
{


	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&](){
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}


	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		UNEYE_PROFILE_FUNCTION();

		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = newEntity;
		}


		// Copy components
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
	
		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>((name.empty()) ? "Entity" : name);

		m_EntityMap[uuid] = entity;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		OnPhysics2DStart();

		// Scripting
		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto entt : view)
			{
				Entity entity = { entt, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		OnPhysics2DStop();

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		m_IsRunning = false;

		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		UNEYE_PROFILE_FUNCTION();

		float currentTime = Time::GetTime();
		if (currentTime - m_LastTime >= 1.0f)
		{
			m_FPS = m_FPSCounter / (currentTime - m_LastTime);
			m_LastTime = currentTime;
			m_FPSCounter = 0;
		}

		RenderScene(camera);

		m_FPSCounter++;
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		UNEYE_PROFILE_FUNCTION();
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Update Script
			{
				// C# script - Entity OnUpdate
				auto view = m_Registry.view<ScriptComponent>();
				for (auto entt : view)
				{
					Entity entity = { entt, this };
					ScriptEngine::OnUpdateEntity(entity, ts);
				}

				m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					// TODO: Move to Scene::OnScenePlay
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entity, this };
						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(ts);
				});
			}

			// Physics
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;

				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve from Box2D
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto entt : view)
				{
					Entity entity = { entt, this };
					auto& tc = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;

					const auto& position = body->GetPosition();
					tc.Translation.x = position.x;
					tc.Translation.y = position.y;
					tc.Rotation.z = body->GetAngle();

				}
			}
		}


		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			// Draw Sprite
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);

					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity, Handle);
				}
			}

			// Draw Circle
			{
				auto view = m_Registry.view<TransformComponent, CircleComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleComponent>(entity);


					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity, Handle);
				}
			}

			// Draw text
			{
				auto view = m_Registry.view<TransformComponent, TextComponent>();
				for (auto entity : view)
				{
					auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

					Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity, Handle);
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		UNEYE_PROFILE_FUNCTION();

		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			float currentTime = Time::GetTime();
			if (currentTime - m_LastTime >= 1.0f)
			{
				m_FPS = m_FPSCounter / (currentTime - m_LastTime);
				m_LastTime = currentTime;
				m_FPSCounter = 0;
			}

			// Physics
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;

				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve from Box2D
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto entt : view)
				{
					Entity entity = { entt, this };
					auto& tc = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;

					const auto& position = body->GetPosition();
					tc.Translation.x = position.x;
					tc.Translation.y = position.y;
					tc.Rotation.z = body->GetAngle();

				}
			}
		}

		// Render
		RenderScene(camera);
		if (!m_IsPaused || m_StepFrames-- > 0)
			m_FPSCounter++;
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		UNEYE_PROFILE_FUNCTION();

		if ((width > 0 && height > 0) && (m_ViewportWidth != width && m_ViewportHeight != height))
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;

			// Resize non-FixedAspectRatio cameras
			auto view = m_Registry.view<CameraComponent>();
			for (auto entity : view)
			{
				auto& cameraComponent = view.get<CameraComponent>(entity);
				if (!cameraComponent.FixedAspectRatio)
				{
					cameraComponent.Camera.SetViewportSize(width, height);
				}
			}
		}
	}

	Entity Scene::DuplicateEntity(Entity entt)
	{
		Entity newEntity = CreateEntity(entt.GetName());
		CopyComponentIfExists(AllComponents{}, newEntity, entt);
		return newEntity;
	}

	Entity Scene::FindFirstEntityByName(std::string_view name)
	{

		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			auto& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}


		return {};
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		// TODO: Maybe assert
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return m_EntityMap[uuid];
		
		return {};

	}


	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entt : view)
		{
			const auto& camera = view.get<CameraComponent>(entt);
			if (camera.Primary)
				return Entity{ entt, this };
		}

		return Entity();
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		//m_PhysicsWorld->CreateBody();
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto entt : view)
		{
			Entity entity = { entt, this };
			auto& tc = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(tc.Translation.x, tc.Translation.y);
			bodyDef.angle = tc.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * tc.Scale.x, bc2d.Size.y * tc.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = tc.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw Sprite
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);


				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity, Handle);
			}
		}

		// Draw Circle
		{
			auto view = m_Registry.view<TransformComponent, CircleComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleComponent>(entity);


				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity, Handle);
			}
		}

		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
				Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity, Handle);
			}
		}

		Renderer2D::EndScene();
	}


	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
		OnComponentAddedImpl(entity, component);
	}

	template<typename T, typename>
	void Scene::OnComponentAddedImpl(Entity entity, T& component) {
		// T don't have OnComponentAdded
	}

	template<typename T>
	void Scene::OnComponentAddedImpl(Entity entity, T& component,
		typename std::enable_if<has_OnComponentAdded_in_scene<Scene, void(Entity, T&)>::value>::type*) {
		// T have OnComponentAdded
		OnComponentAddedImpl<T>(entity, component);
	}

	template<>
	void Scene::OnComponentAddedImpl<CameraComponent>(Entity entity, CameraComponent& component) {
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
}
