#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Scene/SceneCamera.h"

#include "Uneye/Renderer/Texture.h"
#include "Uneye/Renderer/SubTexture.h"
#include "Uneye/Core/UUID.h"
#include "Uneye/Renderer/Font.h"

#include "Uneye/Asset/TextureImporter.h"

#include <entt/entt.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>




namespace Uneye
{
	#define UPDATE_TRANSFORM(p_oldparent, p_curparent, p_cur)\
	{\
		glm::vec3 relative;\
		relative.x = std::abs(p_cur.x - p_oldparent.x);\
		relative.y = std::abs(p_cur.y - p_oldparent.y);\
		relative.z = std::abs(p_cur.z - p_oldparent.z);\
		p_cur.x = (p_oldparent.x <= p_cur.x) ? p_curparent.x + relative.x : p_curparent.x - relative.x;\
		p_cur.y = (p_oldparent.y <= p_cur.y) ? p_curparent.y + relative.y : p_curparent.y - relative.y;\
		p_cur.z = (p_oldparent.z <= p_cur.z) ? p_curparent.z + relative.z : p_curparent.z - relative.z;\
	}


	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& p_id)
			:ID(p_id) {}

	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& p_tag)
			:Tag(p_tag) {}

	};

	struct TransformComponent
	{
		glm::vec3 Translation{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& p_translation)
			:Translation(p_translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation) *
				rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

	};

	struct RelationshipComponent
	{
		entt::entity Parent = entt::null;
		std::vector<entt::entity> Childs = {};

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;

		void AddChild(entt::registry& p_registry, entt::entity p_parent, entt::entity p_child)
		{
			auto& parentRelationship = p_registry.get<RelationshipComponent>(p_parent);

			auto& childRelationship = p_registry.get<RelationshipComponent>(p_child);
			childRelationship.Parent = p_parent;

			auto& parentTC = p_registry.get<TransformComponent>(p_parent);

			childRelationship.m_OldParentPosition = parentTC.Translation;
			childRelationship.m_OldParentRotation = parentTC.Rotation;
			childRelationship.m_OldParentScale = parentTC.Scale;

			parentRelationship.Childs.push_back(p_child);
		}

		void UpdateTransforms(entt::registry& p_registry, entt::entity p_entity) {

			auto& rc = p_registry.get<RelationshipComponent>(p_entity);
			if (rc.Parent != entt::null)
			{
				auto& transform = p_registry.get<TransformComponent>(p_entity);
				auto& parentTransform = p_registry.get<TransformComponent>(rc.Parent);

				UPDATE_TRANSFORM(rc.m_OldParentPosition, parentTransform.Translation, transform.Translation);
				UPDATE_TRANSFORM(rc.m_OldParentRotation, parentTransform.Rotation,	  transform.Rotation);
				UPDATE_TRANSFORM(rc.m_OldParentScale,	 parentTransform.Scale,		  transform.Scale);
				
				rc.m_OldParentPosition = parentTransform.Translation;
				rc.m_OldParentRotation = parentTransform.Rotation;
				rc.m_OldParentScale = parentTransform.Scale;
			}

			if (rc.Childs.empty())
				return;

			for (auto child : rc.Childs) 
			{
				UpdateTransforms(p_registry, child);
			}
		}

		private:
			glm::vec3 m_OldParentPosition;
			glm::vec3 m_OldParentRotation;
			glm::vec3 m_OldParentScale;
	};

	struct SpriteComponent
	{
		glm::vec4 Color = { 1, 1, 1, 1 };
		std::string TexturePath = "assets/textures/checkerboard.png";
		AssetHandle Texture = 0;
		bool IsSubTexture = false;
		glm::vec2 TileSize = { 1, 1 };
		glm::vec2 TileCoord = { 0, 0 };
		glm::vec2 SpriteSize = { 1, 1 };

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& p_color, const std::string& p_texturepath = "assets/textures/checkerboard.png")
			:Color(p_color), TexturePath(p_texturepath), IsSubTexture(false) {}

		SpriteComponent(const glm::vec4& p_color, const std::string& p_texturepath, glm::vec2 p_tileSize,
			glm::vec2 p_tilecoord = glm::vec2(0, 0), glm::vec2 p_spriteSize = glm::vec2(1, 1))
			:Color(p_color), TileSize(p_tileSize), TileCoord(p_tileSize), SpriteSize(p_spriteSize), IsSubTexture(true),
			TexturePath(p_texturepath) {}

	};

	struct CircleComponent
	{
		// Circle Stuff
		glm::vec4 Color{ 1, 1, 1, 1 };
		//float Radius = 0.5f;
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleComponent() = default;
		CircleComponent(const CircleComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: thinking about move to scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	class ScriptableEntity;
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) {
				delete nsc->Instance; nsc->Instance = nullptr;
			};
		}
	};




	// Box2D
	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };

		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO: move into physics material in the future
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		// TODO: move into physics material in the future
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	struct TextComponent
	{
		std::string TextString;
		Ref<Font> FontAsset = Font::GetDefault();
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};

	struct ScriptComponent
	{
		std::string Name;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<RelationshipComponent,TransformComponent, SpriteComponent,
		CircleComponent, CameraComponent, NativeScriptComponent,
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, TextComponent, ScriptComponent>;
}
