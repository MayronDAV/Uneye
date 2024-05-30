#include "uypch.h"
#include "Uneye/Scene/SceneSerializer.h"

#include "Uneye/Scene/Entity.h"
#include "Uneye/Scene/Components.h"
#include "Uneye/Scripting/ScriptEngine.h"
#include "Uneye/Core/UUID.h"
#include "Uneye/Project/Project.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include "Uneye/Utils/YamlTemplates.h"



namespace Uneye
{
	#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
				case ScriptFieldType::FieldType:          \
					out << scriptField.GetValue<Type>();  \
					break

	#define READ_SCRIPT_FIELD(FieldType, Type)             \
		case ScriptFieldType::FieldType:                   \
		{                                                  \
			Type data = scriptField["Data"].as<Type>();    \
			fieldInstance.SetValue(data);                  \
			break;                                         \
		}


	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		UNEYE_PROFILE_FUNCTION();

		UNEYE_CORE_ASSERT(!entity.HasComponent<IDComponent>(), "All entity must to have a UUID!");

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID(); // TODO: Entity ID goes here

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent

			out << YAML::Key << "Name" << YAML::Value << scriptComponent.Name;

			Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.Name);
			const auto& fields = entityClass->GetFields();
			if (fields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields)
				{
					if (entityFields.find(name) == entityFields.end())
						continue;

					out << YAML::BeginMap; // ScriptField
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ConvertScriptFieldMapToString(field.Type);

					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptField = entityFields.at(name);

					switch (field.Type)
					{
						WRITE_SCRIPT_FIELD(Float, float);
						WRITE_SCRIPT_FIELD(Double, double);
						WRITE_SCRIPT_FIELD(Bool, bool);
						WRITE_SCRIPT_FIELD(Char, char);
						WRITE_SCRIPT_FIELD(Byte, int8_t);
						WRITE_SCRIPT_FIELD(Short, int16_t);
						WRITE_SCRIPT_FIELD(Int, int32_t);
						WRITE_SCRIPT_FIELD(Long, int64_t);
						WRITE_SCRIPT_FIELD(UByte, uint8_t);
						WRITE_SCRIPT_FIELD(UShort, uint16_t);
						WRITE_SCRIPT_FIELD(UInt, uint32_t);
						WRITE_SCRIPT_FIELD(ULong, uint64_t);
						WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
						WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
						WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
						WRITE_SCRIPT_FIELD(Entity, UUID);
					}
					out << YAML::EndMap; // ScriptFields
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap; // ScriptComponent
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap; // SpriteComponent

			auto& spriteComponent = entity.GetComponent<SpriteComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteComponent.Color;
			out << YAML::Key << "TexturePath" << YAML::Value << spriteComponent.TexturePath;
			out << YAML::Key << "IsSubTexture" << YAML::Value << spriteComponent.IsSubTexture;

			if (spriteComponent.IsSubTexture)
			{
				out << YAML::Key << "TileSize" << YAML::Value << spriteComponent.TileSize;
				out << YAML::Key << "Coords" << YAML::Value << spriteComponent.Coords;
				out << YAML::Key << "SpriteSize" << YAML::Value << spriteComponent.SpriteSize;
			}

			out << YAML::EndMap; // SpriteComponent
		}

		if (entity.HasComponent<CircleComponent>())
		{
			out << YAML::Key << "CircleComponent";
			out << YAML::BeginMap; // CircleComponent

			auto& circleComponent = entity.GetComponent<CircleComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleComponent.Color;
			out << YAML::Key << "Thickness" << YAML::Value << circleComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleComponent.Fade;

			out << YAML::EndMap; // CircleComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "Type" << YAML::Value << (int)rb2d.Type;
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.FixedRotation;

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2d.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2d.Size;

			out << YAML::Key << "Density" << YAML::Value << bc2d.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2d.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2d.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2d.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& bc2d = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2d.Offset;
			out << YAML::Key << "Radius" << YAML::Value << bc2d.Radius;

			out << YAML::Key << "Density" << YAML::Value << bc2d.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2d.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2d.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2d.RestitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		UNEYE_CORE_ASSERT(true, "Not implemented!");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			UNEYE_CORE_ERROR("Failed to load .uyscene file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		UNEYE_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				UNEYE_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.Name = scriptComponent["Name"].as<std::string>();

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.Name);
						UNEYE_CORE_ASSERT(!entityClass);
						const auto& fields = entityClass->GetFields();
						auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

						for (auto scriptField : scriptFields)
						{
							std::string name = scriptField["Name"].as<std::string>();
							std::string typeString = scriptField["Type"].as<std::string>();
							ScriptFieldType type = Utils::ConvertStringToScriptFieldMap(typeString);

							ScriptFieldInstance& fieldInstance = entityFields[name];

							// TODO: turn this assert into log warning
							//UNEYE_CORE_ASSERT(fields.find(name) == fields.end());

							if (fields.find(name) == fields.end())
								continue;

							fieldInstance.Field = fields.at(name);

							switch (type)
							{
								READ_SCRIPT_FIELD(Float, float);
								READ_SCRIPT_FIELD(Double, double);
								READ_SCRIPT_FIELD(Bool, bool);
								READ_SCRIPT_FIELD(Char, char);
								READ_SCRIPT_FIELD(Byte, int8_t);
								READ_SCRIPT_FIELD(Short, int16_t);
								READ_SCRIPT_FIELD(Int, int32_t);
								READ_SCRIPT_FIELD(Long, int64_t);
								READ_SCRIPT_FIELD(UByte, uint8_t);
								READ_SCRIPT_FIELD(UShort, uint16_t);
								READ_SCRIPT_FIELD(UInt, uint32_t);
								READ_SCRIPT_FIELD(ULong, uint64_t);
								READ_SCRIPT_FIELD(Vector2, glm::vec2);
								READ_SCRIPT_FIELD(Vector3, glm::vec3);
								READ_SCRIPT_FIELD(Vector4, glm::vec4);
								READ_SCRIPT_FIELD(Entity, UUID);
							}
						}
					}
				}

				auto spriteComponent = entity["SpriteComponent"];
				if (spriteComponent)
				{
					auto& mc = deserializedEntity.AddComponent<SpriteComponent>();
					mc.Color = spriteComponent["Color"].as<glm::vec4>();
					mc.TexturePath = spriteComponent["TexturePath"].as<std::string>();
					if (mc.TexturePath == "" || mc.TexturePath == " " || mc.TexturePath == std::string())
					{
						mc.Texture = nullptr;
						mc.IsSubTexture = false;
					}
					else
					{
						//mc.TexturePath = Project::GetAssetFileSystemPath(mc.TexturePath).string();
						//auto assetPath = Project::GetProjectDirectory() / Project::GetAssetDirectory();
						//mc.TexturePath = std::filesystem::relative(mc.TexturePath, assetPath).string();
						//mc.TexturePath = Project::GetAssetFileSystemPath(mc.TexturePath).string();

						mc.Texture = Texture2D::Create(mc.TexturePath);
					}

					mc.IsSubTexture = spriteComponent["IsSubTexture"].as<bool>();

					if (mc.IsSubTexture)
					{
						mc.TileSize = spriteComponent["TileSize"].as<glm::vec2>();
						mc.Coords = spriteComponent["Coords"].as<glm::vec2>();
						mc.SpriteSize = spriteComponent["SpriteSize"].as<glm::vec2>();

						mc.SubTexture = SubTexture2D::CreateFromTexture(mc.Texture, mc.Coords,
							mc.TileSize, mc.SpriteSize);
					}
				}

				auto circleComponent = entity["CircleComponent"];
				if (circleComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CircleComponent>();
					cc.Color = circleComponent["Color"].as<glm::vec4>();
					cc.Thickness = circleComponent["Thickness"].as<float>();
					cc.Fade = circleComponent["Fade"].as<float>();
				}

				auto rb2dcomponent = entity["Rigidbody2DComponent"];
				if (rb2dcomponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = (Rigidbody2DComponent::BodyType)rb2dcomponent["Type"].as<int>();
					rb2d.FixedRotation = rb2dcomponent["FixedRotation"].as<bool>();
				}

				auto bc2dcomponent = entity["BoxCollider2DComponent"];
				if (bc2dcomponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = bc2dcomponent["Offset"].as<glm::vec2>();
					bc2d.Size = bc2dcomponent["Size"].as<glm::vec2>();

					bc2d.Density = bc2dcomponent["Density"].as<float>();
					bc2d.Friction = bc2dcomponent["Friction"].as<float>();
					bc2d.Restitution = bc2dcomponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = bc2dcomponent["RestitutionThreshold"].as<float>();

				}

				auto cc2dcomponent = entity["CircleCollider2DComponent"];
				if (cc2dcomponent)
				{
					auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2d.Offset = cc2dcomponent["Offset"].as<glm::vec2>();
					cc2d.Radius = cc2dcomponent["Radius"].as<float>();

					cc2d.Density = cc2dcomponent["Density"].as<float>();
					cc2d.Friction = cc2dcomponent["Friction"].as<float>();
					cc2d.Restitution = cc2dcomponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = cc2dcomponent["RestitutionThreshold"].as<float>();

				}
			}
		}

		UNEYE_CORE_INFO("Deserialized scene '{0}' successfully", sceneName);
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		UNEYE_CORE_ASSERT(true, "Not implemented!");
		return false;
	}

}
