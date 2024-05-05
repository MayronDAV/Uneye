#pragma once

#include "Uneye/Core/Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Uneye {

	class Log
	{
		public:
			static void Init();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define UNEYE_CORE_TRACE(...)   	::Uneye::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define UNEYE_CORE_INFO(...)    	::Uneye::Log::GetCoreLogger()->info(__VA_ARGS__)
#define UNEYE_CORE_WARN(...)    	::Uneye::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define UNEYE_CORE_ERROR(...)   	::Uneye::Log::GetCoreLogger()->error(__VA_ARGS__)
#define UNEYE_CORE_CRITICAL(...)	::Uneye::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define UNEYE_TRACE(...)			::Uneye::Log::GetClientLogger()->trace(__VA_ARGS__)
#define UNEYE_INFO(...)	      		::Uneye::Log::GetClientLogger()->info(__VA_ARGS__)
#define UNEYE_WARN(...)				::Uneye::Log::GetClientLogger()->warn(__VA_ARGS__)
#define UNEYE_ERROR(...)			::Uneye::Log::GetClientLogger()->error(__VA_ARGS__)
#define UNEYE_CRITICAL(...)			::Uneye::Log::GetClientLogger()->critical(__VA_ARGS__)