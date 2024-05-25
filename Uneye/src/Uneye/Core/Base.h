#pragma once

#include "Uneye/Core/Log.h"

#include <memory>
#include <filesystem>

#pragma region Platform and other stuff

#ifdef UNEYE_PLATFORM_WINDOWS
	#define UNEYE_DEBUGBREAK() __debugbreak()
#else
	#error Uneye only supports Windows!
#endif


#ifdef UNEYE_DEBUG
	#define UNEYE_ENABLE_ASSERTS
#endif

#define UNEYE_EXPAND_MACRO(x) x
#define UNEYE_STRINGIFY_MACRO(x) #x

#pragma endregion

#pragma region Assert and Verify

#ifdef UNEYE_ENABLE_ASSERTS
	#define UNEYE_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(check) { UNEYE##type##ERROR(msg, __VA_ARGS__); UNEYE_DEBUGBREAK(); } }
	#define UNEYE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) UNEYE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define UNEYE_INTERNAL_ASSERT_NO_MSG(type, check) UNEYE_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", UNEYE_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define UNEYE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define UNEYE_INTERNAL_ASSERT_GET_MACRO(...) UNEYE_EXPAND_MACRO( UNEYE_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, UNEYE_INTERNAL_ASSERT_WITH_MSG, UNEYE_INTERNAL_ASSERT_NO_MSG) )

	#define UNEYE_ASSERT(...) UNEYE_EXPAND_MACRO( UNEYE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define UNEYE_CORE_ASSERT(...) UNEYE_EXPAND_MACRO( UNEYE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define UNEYE_ASSERT(...)
	#define UNEYE_CORE_ASSERT(...)
#endif

#pragma endregion

#define BIT(x) (1 << x)

#define UNEYE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


namespace Uneye
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
