#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Scene/SceneCamera.h"

#include "Uneye/Renderer/Texture.h"
#include "Uneye/Renderer/SubTexture.h"
#include "Uneye/Core/UUID.h"

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
		IDComponent(const UUID& id)
			:ID(id) {}

	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:Tag(tag) {}

	};

	struct TransformComponent
	{
		glm::vec3 Translation{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			:Translation(translation) {}

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
		Ref<Texture2D> Texture = Texture2D::Create(TexturePath);
		Ref<SubTexture2D> SubTexture = nullptr;
		bool IsSubTexture = false;
		glm::vec2 TileSize = { 1, 1 }; // Tamanho geral de cada tile da textura.
		glm::vec2 Coords = { 0, 0 }; // Coords dentro da textura
		glm::vec2 SpriteSize = { 1, 1 }; // Mudar o nome

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& color, const std::string& texturepath = "assets/textures/checkerboard.png")
			:Color(color), TexturePath(texturepath), Texture(Texture2D::Create(TexturePath)), IsSubTexture(false) {}

		SpriteComponent(const glm::vec4& color, const std::string& texturepath, glm::vec2 tileSize,
			glm::vec2 coords = glm::vec2(0, 0), glm::vec2 spriteSize = glm::vec2(1, 1))
			:Color(color), TileSize(tileSize), Coords(coords), SpriteSize(spriteSize), IsSubTexture(true),
			TexturePath(texturepath), Texture(Texture2D::Create(TexturePath)), SubTexture(SubTexture2D::CreateFromTexture(Texture, coords, tileSize, spriteSize)) {}

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

}
