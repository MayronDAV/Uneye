#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Scene/SceneCamera.h"
#include "Uneye/Scene/ScriptableEntity.h"

#include "Uneye/Renderer/Texture.h"
#include "Uneye/Renderer/SubTexture.h"


namespace Uneye
{
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

	struct MaterialComponent
	{
		glm::vec4 Color = { 1, 1, 1, 1 };
		std::string TexturePath = "assets/textures/checkerboard.png";
		Ref<Texture2D> Texture = Texture2D::Create(TexturePath);
		Ref<SubTexture2D> SubTexture = nullptr;
		bool IsSubTexture = false;
		glm::vec2 TileSize = { 1, 1 }; // Tamanho geral de cada tile da textura.
		glm::vec2 Coords = { 0, 0 }; // Coords dentro da textura
		glm::vec2 SpriteSize = { 1, 1 }; // Mudar o nome

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(const glm::vec4& color, const std::string& texturepath = "assets/textures/checkerboard.png")
			:Color(color), TexturePath(texturepath), Texture(Texture2D::Create(TexturePath)), IsSubTexture(false) {}

		MaterialComponent(const glm::vec4& color, const std::string& texturepath, glm::vec2 tileSize,
			glm::vec2 coords = glm::vec2(0, 0), glm::vec2 spriteSize = glm::vec2(1, 1))
			:Color(color), TileSize(tileSize), Coords(coords), SpriteSize(spriteSize), IsSubTexture(true),
			TexturePath(texturepath), Texture(Texture2D::Create(TexturePath)), SubTexture(SubTexture2D::CreateFromTexture(Texture, coords, tileSize, spriteSize)) {}

	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: thinking about move to scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

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



}
