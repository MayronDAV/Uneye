#include "uypch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"
#include "Uneye/Core/UUID.h"
#include "Uneye/Scene/SceneManager.h"

#include "Uneye/Core/KeyCodes.h"
#include "Uneye/Core/Input.h"

#include "Uneye/Physics/Physics2D.h"

#include <glm/glm.hpp>

#include "mono/metadata/object.h"
#include <mono/metadata/reflection.h>

#include <box2d/b2_body.h>




namespace Uneye
{
	#pragma region  Utils

	static std::string MonoStringToString(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		return str;
	}

	#define UNEYE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Uneye.InternalCalls::"#Name, (void*)InternalCalls::Name)
	
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentsFuncs;

	template<typename... TComponent>
	static void RegisterComponent()
	{
		([&]()
			{
				std::string_view typeName = typeid(TComponent).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);

				std::string managedTypeName = fmt::format("Uneye.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), ScriptEngine::GetCoreAssemblyImage());

				if (!managedType)
				{
					UNEYE_CORE_ERROR("Could not find component type {}", managedTypeName);
					return;
				}

				s_EntityHasComponentsFuncs[managedType] = [](Entity entt) { return entt.HasComponent<TComponent>(); };

			}(), ...);
	}

	template<typename... TComponent>
	static void RegisterComponent(ComponentGroup<TComponent...>)
	{
		RegisterComponent<TComponent...>();
	}

	#pragma endregion

	namespace InternalCalls
	{
		#pragma region Log Calls

		static void NativeLog(MonoString * message)
		{
			auto str = MonoStringToString(message);

			UNEYE_INFO(str);
		}

		#pragma endregion

		#pragma region Entity And Components Calls
		
		#pragma region Entity

		static bool Entity_HasComponent(UUID enttID, MonoReflectionType* componentType)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				MonoType* managedType = mono_reflection_type_get_type(componentType);
				UNEYE_CORE_ASSERT(s_EntityHasComponentsFuncs.find(managedType) == s_EntityHasComponentsFuncs.end());

				return s_EntityHasComponentsFuncs.at(managedType)(entt);
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
			return false;
		}

		static uint64_t Entity_FindFirstEntityByName(MonoString* name)
		{
			char* name_c_str = mono_string_to_utf8(name);

			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->FindFirstEntityByName(name_c_str);
				mono_free(name_c_str);

				if (!entt)
					continue;

				return entt.GetUUID();
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
			return 0;
		}

		static MonoObject* GetScriptInstance(UUID entityID)
		{
			return ScriptEngine::GetManageInstance(entityID);
		}

		#pragma endregion 

		#pragma region TransformComponent

		static void TransformComponent_GetTranslation(UUID enttID, glm::vec3* outTranslation)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				*outTranslation = entt.GetComponent<TransformComponent>().Translation;
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static void TransformComponent_SetTranslation(UUID enttID,  glm::vec3* translation)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);		
				if (!entt)
					continue;

				entt.GetComponent<TransformComponent>().Translation = *translation;
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		#pragma endregion

		#pragma region Rigidbody2DComponent

		static void Rigidbody2DComponent_GetTranslation(UUID enttID, glm::vec2* outTranslation)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;

				*outTranslation = glm::vec2(body->GetTransform().p.x, body->GetTransform().p.y);
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static void Rigidbody2DComponent_SetTransform(UUID enttID, glm::vec2* position, float angle)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;

				body->SetTransform(b2Vec2(position->x, position->y), angle);
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static void Rigidbody2DComponent_ApplyLinearImpulse(UUID enttID, glm::vec2* impulse, glm::vec2* point, bool wake)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;

				b2Vec2 b2_impulse(impulse->x, impulse->y);
				b2Vec2 b2_point(point->x, point->y);
				body->ApplyLinearImpulse(b2_impulse, b2_point, wake);
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID enttID, glm::vec2* impulse, bool wake)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;

				b2Vec2 b2_impulse(impulse->x, impulse->y);
				body->ApplyLinearImpulseToCenter(b2_impulse, wake);
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static void Rigidbody2DComponent_GetLinearVelocity(UUID enttID, glm::vec2* outLinearVelocity)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const b2Vec2& linearVelocity = body->GetLinearVelocity();
				*outLinearVelocity = glm::vec2(linearVelocity.x, linearVelocity.y);
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static Rigidbody2DComponent::BodyType Rigidbody2DComponent_GetType(UUID enttID)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				return Utils::Rigidbody2DTypeFromBox2DBody(body->GetType());
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
			return Rigidbody2DComponent::BodyType::Static;
		}

		static void Rigidbody2DComponent_SetType(UUID enttID, Rigidbody2DComponent::BodyType bodyType)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				body->SetType(Utils::Rigidbody2DTypeToBox2DBody(bodyType));
				return;
			}
		}

		#pragma endregion

		#pragma region TextComponent

		static MonoString* TextComponent_GetText(UUID enttID)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(!entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				return ScriptEngine::CreateString(tc.TextString.c_str());
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
			return ScriptEngine::CreateString("Null");
		}

		static void TextComponent_SetText(UUID enttID, MonoString* textString)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(!entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				tc.TextString = MonoStringToString(textString);
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static void TextComponent_GetColor(UUID enttID, glm::vec4* color)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(!entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				*color = tc.Color;
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static void TextComponent_SetColor(UUID enttID, glm::vec4* color)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(!entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				tc.Color = *color;
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static float TextComponent_GetKerning(UUID enttID)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(!entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				return tc.Kerning;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
			return 0.0f;
		}

		static void TextComponent_SetKerning(UUID enttID, float kerning)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(!entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				tc.Kerning = kerning;
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		static float TextComponent_GetLineSpacing(UUID enttID)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				return tc.LineSpacing;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
			return 0.0f;
		}

		static void TextComponent_SetLineSpacing(UUID enttID, float lineSpacing)
		{
			for (const auto& [path, scene] : SceneManager::GetScenes())
			{
				Entity entt = scene->GetEntityByUUID(enttID);
				if (!entt)
					continue;

				UNEYE_CORE_ASSERT(!entt.HasComponent<TextComponent>());

				auto& tc = entt.GetComponent<TextComponent>();
				tc.LineSpacing = lineSpacing;
				return;
			}

			UNEYE_CORE_ERROR("Unkwnown entity ID");
		}

		#pragma endregion

		#pragma endregion

		#pragma region Events

		static bool Input_IsKeyDown(KeyCode keycode)
		{
			return Input::IsKeyPressed(keycode);
		}

		#pragma endregion
	}


	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentsFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunction()
	{
		#pragma region Log Calls

		UNEYE_ADD_INTERNAL_CALL(NativeLog);

		#pragma endregion

		#pragma region Entity And Components Calls

		#pragma region Entity

		UNEYE_ADD_INTERNAL_CALL(Entity_HasComponent);
		UNEYE_ADD_INTERNAL_CALL(Entity_FindFirstEntityByName);
		UNEYE_ADD_INTERNAL_CALL(GetScriptInstance);

		#pragma endregion

		#pragma region TransformComponent

		UNEYE_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		UNEYE_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		#pragma endregion

		#pragma region Rigidbody2DComponent

		//UNEYE_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetTranslation);
		UNEYE_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetTransform);
		UNEYE_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		UNEYE_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
		UNEYE_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
		UNEYE_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
		UNEYE_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);


		#pragma endregion

		#pragma region TextComponent

		UNEYE_ADD_INTERNAL_CALL(TextComponent_GetText);
		UNEYE_ADD_INTERNAL_CALL(TextComponent_SetText);
		UNEYE_ADD_INTERNAL_CALL(TextComponent_GetColor);
		UNEYE_ADD_INTERNAL_CALL(TextComponent_SetColor);
		UNEYE_ADD_INTERNAL_CALL(TextComponent_GetKerning);
		UNEYE_ADD_INTERNAL_CALL(TextComponent_SetKerning);
		UNEYE_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
		UNEYE_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);

		#pragma endregion

		#pragma endregion

		#pragma region Events

		UNEYE_ADD_INTERNAL_CALL(Input_IsKeyDown);

		#pragma endregion

	}

}
