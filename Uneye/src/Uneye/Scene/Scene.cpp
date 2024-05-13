#include "uypch.h"
#include "Uneye/Scene/Scene.h"

#include "Uneye/Scene/Components.h"
#include "Uneye/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Uneye/Scene/Entity.h"


namespace Uneye
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>((name.empty()) ? "Entity" : name);

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		UNEYE_PROFILE_FUNCTION();

		Renderer2D::BeginScene(camera);

		auto groupM = m_Registry.group<TransformComponent>(entt::get<MaterialComponent>);
		for (auto entity : groupM)
		{
			auto [transform, material] = groupM.get<TransformComponent, MaterialComponent>(entity);


			Renderer2D::DrawSprite(transform.GetTransform(), material, (int)entity);
		}

		Renderer2D::EndScene();
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

			auto groupM = m_Registry.group<TransformComponent>(entt::get<MaterialComponent>);
			for (auto entity : groupM)
			{
				auto [transform, material] = groupM.get<TransformComponent, MaterialComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), material, (int)entity);
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
		static_assert(false);
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
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}


}
