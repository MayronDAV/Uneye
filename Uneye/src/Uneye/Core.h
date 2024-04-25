#pragma once

#ifdef UNEYE_PLATFORM_WINDOWS
#ifdef UNEYE_DYNAMIC_LINK
	#ifdef UNEYE_BUILD_DLL
		#define UNEYE_API __declspec(dllexport)
	#else
		#define UNEYE_API __declspec(dllimport)
	#endif
#else
    #define UNEYE_API
#endif
#else
	#error Uneye only supports Windows!
#endif

#ifdef UNEYE_DEBUG
	#define UNEYE_ENABLE_ASSERTS
#endif

#ifdef UNEYE_ENABLE_ASSERTS
	#define UNEYE_ASSERT(x, ...) { if(!(x)) { UNEYE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define UNEYE_CORE_ASSERT(x, ...) { if(!(x)) { UNEYE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define UNEYE_ASSERT(x, ...)
	#define UNEYE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define UNEYE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)