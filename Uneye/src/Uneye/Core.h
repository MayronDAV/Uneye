#pragma once

#ifdef UY_PLATFORM_WINDOWS

	#ifdef UY_BUILD_DLL
		#define UY_API __declspec(dllexport)
	#else
		#define UY_API __declspec(dllimport)
	#endif

#else
	#error Uneye Engine only support windows
#endif
