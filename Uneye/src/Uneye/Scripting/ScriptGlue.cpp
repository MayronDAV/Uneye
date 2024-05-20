#include "uypch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"
#include "Uneye/Core/UUID.h"

#include "Uneye/Core/KeyCodes.h"
#include "Uneye/Core/Input.h"

#include "mono/metadata/object.h"

#include <glm/glm.hpp>



namespace Uneye
{

	#define UNEYE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Uneye.InternalCalls::"#Name, (void*)InternalCalls::Name)
	
	namespace InternalCalls
	{
		#pragma region LogCalls

		static void NativeLog(MonoString * message)
		{
			char* cStr = mono_string_to_utf8(message);
			std::string str(cStr);
			mono_free(cStr);

			UNEYE_INFO(str);
		}

		#pragma endregion


		#pragma region EntityCalls
		
		static void Entity_GetTranslation(UUID enttID, glm::vec3* outTranslation)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			Entity entt = scene->GetEntityByUUID(enttID);
			*outTranslation = entt.GetComponent<TransformComponent>().Translation;
			//UNEYE_CORE_WARN("Entity_GetTranslation ID: {0}", entt.GetUUID());
		}

		static void Entity_SetTranslation(UUID enttID,  glm::vec3* translation)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			Entity entt = scene->GetEntityByUUID(enttID);
			entt.GetComponent<TransformComponent>().Translation = *translation;
			//UNEYE_CORE_WARN("Entity_SetTranslation ID: {0}", entt.GetUUID());
		}

		#pragma endregion

		#pragma region Events

		static bool Input_IsKeyDown(KeyCode keycode)
		{
			return Input::IsKeyPressed(keycode);
		}

		#pragma endregion
	}


	void ScriptGlue::RegisterFunction()
	{
		#pragma region LogCalls

		UNEYE_ADD_INTERNAL_CALL(NativeLog);

		#pragma endregion

		#pragma region EntityCalls

		UNEYE_ADD_INTERNAL_CALL(Entity_GetTranslation);
		UNEYE_ADD_INTERNAL_CALL(Entity_SetTranslation);

		#pragma endregion

		#pragma region Events

		UNEYE_ADD_INTERNAL_CALL(Input_IsKeyDown);

		#pragma endregion

	}
}
