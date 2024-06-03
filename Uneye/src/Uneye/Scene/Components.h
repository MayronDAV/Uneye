#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Scene/SceneCamera.h"

#include "Uneye/Renderer/Texture.h"
#include "Uneye/Renderer/SubTexture.h"
#include "Uneye/Core/UUID.h"
#include "Uneye/Renderer/Font.h"

#include "Uneye/Asset/TextureImporter.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>




namespace Uneye
{
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
		ComponentGroup<TransformComponent, SpriteComponent,
		CircleComponent, CameraComponent, NativeScriptComponent,
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, TextComponent, ScriptComponent>;
}
