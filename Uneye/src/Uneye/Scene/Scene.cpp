#include "uypch.h"
#include "Uneye/Scene/Scene.h"

#include "Uneye/Scene/Components.h"
#include "Uneye/Scene/ScriptableEntity.h"
#include "Uneye/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Uneye/Scene/Entity.h"
#include <chrono>
#include "Uneye/Core/Timer.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"


namespace Uneye
{
	namespace Utils
	{
		static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
				case Uneye::Rigidbody2DComponent::BodyType::Static: 
					return b2BodyType::b2_staticBody;
				case Uneye::Rigidbody2DComponent::BodyType::Dynamic:
					return b2BodyType::b2_dynamicBody;
				case Uneye::Rigidbody2DComponent::BodyType::Kinematic:
					return b2BodyType::b2_kinematicBody;
			}

			UNEYE_CORE_ASSERT(true, "Unknown body type!");
			return b2BodyType::b2_staticBody;
		}
	}


	Scene::Scene()
	{
	}

	Scene::~Scene()
	{

	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			UNEYE_CORE_ASSERT(enttMap.find(uuid) == enttMap.end(), "This uuid exists?");
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}
	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());

	}


	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
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
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
	
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

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_PhysicsWorld = new b2World({0.0f, -9.8f});
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
				boxShape.SetAsBox(bc2d.Size.x * tc.Scale.x, bc2d.Size.y * tc.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}

	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
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

		Renderer2D::BeginScene(camera);

		// Draw Sprite
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);


				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1, 0, 1, 1));
			}
		}

		// Draw Circle
		{
			auto view = m_Registry.view<TransformComponent, CircleComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleComponent>(entity);


				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		//Renderer2D::DrawLine(glm::vec3(0), glm::vec3(5.0f), glm::vec4(1, 0, 1, 1));

		//Renderer2D::DrawRect(glm::vec3(2.0f, 0, 0), glm::vec2(1.0f));
		//Renderer2D::DrawRect(glm::vec3(4.0f, 0, 0), glm::vec2(1.0f));

		Renderer2D::EndScene();

		m_FPSCounter++;
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		UNEYE_PROFILE_FUNCTION();

		// Update Script
		{
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


					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			// Draw Circle
			{
				auto view = m_Registry.view<TransformComponent, CircleComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleComponent>(entity);


					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}


			Renderer2D::EndScene();
		}

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		UNEYE_PROFILE_FUNCTION();

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

	void Scene::DuplicateEntity(Entity entt)
	{
		Entity newEntity = CreateEntity(entt.GetName());

		CopyComponentIfExists<TransformComponent>(newEntity, entt);
		CopyComponentIfExists<CameraComponent>(newEntity, entt);
		CopyComponentIfExists<SpriteComponent>(newEntity, entt);
		CopyComponentIfExists<CircleComponent>(newEntity, entt);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entt);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entt);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entt);

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

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}


	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteComponent>(Entity entity, SpriteComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleComponent>(Entity entity, CircleComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

}
