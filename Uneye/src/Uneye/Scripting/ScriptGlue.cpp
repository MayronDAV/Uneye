#include "uypch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"
#include "Uneye/Core/UUID.h"

#include "Uneye/Core/KeyCodes.h"
#include "Uneye/Core/Input.h"

#include <glm/glm.hpp>

#include "mono/metadata/object.h"
#include <mono/metadata/reflection.h>

#include <box2d/b2_body.h>




namespace Uneye
{
	#pragma region  Utils

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
			char* cStr = mono_string_to_utf8(message);
			std::string str(cStr);
			mono_free(cStr);

			UNEYE_INFO(str);
		}

		#pragma endregion

		#pragma region Entity And Components Calls
		
		#pragma region Entity

		static bool Entity_HasComponent(UUID enttID, MonoReflectionType* componentType)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->GetEntityByUUID(enttID);
			UNEYE_CORE_ASSERT(!entt, "Entity is null");

			MonoType* managedType = mono_reflection_type_get_type(componentType);
			UNEYE_CORE_ASSERT(s_EntityHasComponentsFuncs.find(managedType) == s_EntityHasComponentsFuncs.end(), "Unknown managed type");

			return s_EntityHasComponentsFuncs.at(managedType)(entt);
		}

		static uint64_t Entity_FindFirstEntityByName(MonoString* name)
		{
			char* name_c_str = mono_string_to_utf8(name);

			Scene* scene = ScriptEngine::GetSceneContext();
			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->FindFirstEntityByName(name_c_str);
			mono_free(name_c_str);

			if (!entt)
				return 0;

			return entt.GetUUID();
		}

		static MonoObject* GetScriptInstance(UUID entityID)
		{
			return ScriptEngine::GetManageInstance(entityID);
		}

		#pragma endregion 

		#pragma region TransformComponent

		static void TransformComponent_GetTranslation(UUID enttID, glm::vec3* outTranslation)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->GetEntityByUUID(enttID);
			UNEYE_CORE_ASSERT(!entt, "Entity is null");

			*outTranslation = entt.GetComponent<TransformComponent>().Translation;
		}

		static void TransformComponent_SetTranslation(UUID enttID,  glm::vec3* translation)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->GetEntityByUUID(enttID);		
			UNEYE_CORE_ASSERT(!entt, "Entity is null");

			entt.GetComponent<TransformComponent>().Translation = *translation;
		}

		#pragma endregion

		#pragma region Rigidbody2DComponent

		static void Rigidbody2DComponent_GetTranslation(UUID enttID, glm::vec2* outTranslation)
		{
			Scene* scene = ScriptEngine::GetSceneContext();

			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->GetEntityByUUID(enttID);

			UNEYE_CORE_ASSERT(!entt, "Entity is null");

			auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;

			*outTranslation = glm::vec2(body->GetTransform().p.x, body->GetTransform().p.y);
		}

		static void Rigidbody2DComponent_SetTransform(UUID enttID, glm::vec2* position, float angle)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->GetEntityByUUID(enttID);
			UNEYE_CORE_ASSERT(!entt, "Entity is null");

			auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;

			body->SetTransform(b2Vec2(position->x, position->y), angle);
		}

		static void Rigidbody2DComponent_ApplyLinearImpulse(UUID enttID, glm::vec2* impulse, glm::vec2* point, bool wake)
		{
			Scene* scene = ScriptEngine::GetSceneContext();

			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->GetEntityByUUID(enttID);

			UNEYE_CORE_ASSERT(!entt, "Entity is null");

			auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;

			b2Vec2 b2_impulse(impulse->x, impulse->y);
			b2Vec2 b2_point(point->x, point->y);
			body->ApplyLinearImpulse(b2_impulse, b2_point, wake);
		}

		static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID enttID, glm::vec2* impulse, bool wake)
		{
			Scene* scene = ScriptEngine::GetSceneContext();

			UNEYE_CORE_ASSERT(scene == nullptr, "Scene is null");

			Entity entt = scene->GetEntityByUUID(enttID);

			UNEYE_CORE_ASSERT(!entt, "Entity is null");

			auto& rb2d = entt.GetComponent<Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;

			b2Vec2 b2_impulse(impulse->x, impulse->y);
			body->ApplyLinearImpulseToCenter(b2_impulse, wake);
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

		#pragma endregion

		#pragma endregion

		#pragma region Events

		UNEYE_ADD_INTERNAL_CALL(Input_IsKeyDown);

		#pragma endregion

	}

}
