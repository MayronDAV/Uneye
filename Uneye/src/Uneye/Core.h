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

#ifdef UNEYE_DEBUG
	#define UNEYE_ENABLE_ASSERTS
#endif

#ifdef UNEYE_ENABLE_ASSERTS
	#define UNEYE_ASSERT(x, ...) {\
		if (x) {\
			 UNEYE_ERROR("Assertion failed: {0}", __VA_ARGS__);\
			__debugbreak();\
		}\
	}
	#define UNEYE_CORE_ASSERT(x, ...) {\
		if (x){\
			UNEYE_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__);\
			__debugbreak();\
		}\
	}
#else
	#define UNEYE_ASSERT(...)
	#define UNEYE_CORE_ASSERT(...)
#endif

#define BIT(x) (1 << x)