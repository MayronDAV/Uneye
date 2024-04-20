#pragma once

#ifdef UNEYE_PLATFORM_WINDOWS

	#ifdef UNEYE_BUILD_DLL
		#define UNEYE_API __declspec(dllexport)
	#else
		#define UNEYE_API __declspec(dllimport)
	#endif

#else
	#error Uneye Engine only support windows
#endif

#define BIT(x) (1 << x)