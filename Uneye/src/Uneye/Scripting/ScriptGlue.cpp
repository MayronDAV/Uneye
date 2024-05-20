#include "uypch.h"
#include "ScriptGlue.h"

#include "mono/metadata/object.h"




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

	}


	void ScriptGlue::RegisterFunction()
	{
		#pragma region LogCalls

			UNEYE_ADD_INTERNAL_CALL(NativeLog);

		#pragma endregion

	}
}
