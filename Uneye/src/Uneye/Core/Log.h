#pragma once

#include "Uneye/Core/Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Uneye 
{

	enum class LogType
	{
		Trace,
		Info,
		Warn,
		Error,
		Critical
	};

	namespace Utils
	{
		template<size_t Buffer_Size>
		inline static std::string_view to_string_view(const fmt::basic_memory_buffer<char, Buffer_Size>& buf)
		{
			return std::string_view(buf.data(), buf.size());
		}
	}

	enum class Logger
	{
		Core,
		App
	};

	class LoggerMessage
	{
		struct LogMsg
		{
			LogType Type;
			std::string Message;
		};

		public:
			LoggerMessage() = default;

			template <typename... Args>
			void trace(const std::string& fmt, const Args & ... args) { SubmitMessage(LogType::Trace, fmt.c_str(), args...); }

			template <typename... Args>
			void info(const std::string& fmt, const Args & ... args) { SubmitMessage(LogType::Info, fmt.c_str(), args...); }

			template <typename... Args>
			void warn(const std::string& fmt, const Args & ... args) { SubmitMessage(LogType::Warn, fmt.c_str(), args...); }

			template <typename... Args>
			void error(const std::string& fmt, const Args & ... args) { SubmitMessage(LogType::Error, fmt.c_str(), args...); }

			template <typename... Args>
			void critical(const std::string& fmt, const Args & ... args) { SubmitMessage(LogType::Critical, fmt.c_str(), args...); }

			const std::vector<LogMsg>& GetMessages() { return s_LogMessages; }

		private:

			template <typename... Args>
			void SubmitMessage(LogType p_type, const char* fmt, const Args & ... args)
			{
				fmt::memory_buffer buf;
				fmt::format_to(buf, fmt, args...);
				std::string message = std::string(buf.data(), buf.size());

				s_LogMessages.push_back(LogMsg{p_type, message});
			}

			static std::vector<LogMsg> s_LogMessages;
	};

	class Log
	{
		public:

			static void Init();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

			inline static std::shared_ptr<LoggerMessage>& GetLoggerMessage() { return s_LoggerMessage; }

		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;

			static std::shared_ptr<LoggerMessage> s_LoggerMessage;
	};
}

#ifndef UNEYE_DIST
	// Core log macros
	#define UNEYE_CORE_TRACE(...)   	::Uneye::Log::GetCoreLogger()->trace(__VA_ARGS__);	    ::Uneye::Log::GetLoggerMessage()->trace( __VA_ARGS__)
	#define UNEYE_CORE_INFO(...)    	::Uneye::Log::GetCoreLogger()->info(__VA_ARGS__);	    ::Uneye::Log::GetLoggerMessage()->info( __VA_ARGS__)
	#define UNEYE_CORE_WARN(...)    	::Uneye::Log::GetCoreLogger()->warn(__VA_ARGS__);	    ::Uneye::Log::GetLoggerMessage()->warn( __VA_ARGS__)
	#define UNEYE_CORE_ERROR(...)   	::Uneye::Log::GetCoreLogger()->error(__VA_ARGS__);	    ::Uneye::Log::GetLoggerMessage()->error( __VA_ARGS__)
	#define UNEYE_CORE_CRITICAL(...)	::Uneye::Log::GetCoreLogger()->critical(__VA_ARGS__);   ::Uneye::Log::GetLoggerMessage()->critical( __VA_ARGS__)

	// Client log macros
	#define UNEYE_TRACE(...)			::Uneye::Log::GetClientLogger()->trace(__VA_ARGS__);	::Uneye::Log::GetLoggerMessage()->trace( __VA_ARGS__)
	#define UNEYE_INFO(...)	      		::Uneye::Log::GetClientLogger()->info(__VA_ARGS__);		::Uneye::Log::GetLoggerMessage()->info( __VA_ARGS__)
	#define UNEYE_WARN(...)				::Uneye::Log::GetClientLogger()->warn(__VA_ARGS__);		::Uneye::Log::GetLoggerMessage()->warn( __VA_ARGS__)
	#define UNEYE_ERROR(...)			::Uneye::Log::GetClientLogger()->error(__VA_ARGS__);	::Uneye::Log::GetLoggerMessage()->error( __VA_ARGS__)
	#define UNEYE_CRITICAL(...)			::Uneye::Log::GetClientLogger()->critical(__VA_ARGS__); ::Uneye::Log::GetLoggerMessage()->critical( __VA_ARGS__)
#else
	// Core log macros
	#define UNEYE_CORE_TRACE(...)    ::Uneye::Log::GetLoggerMessage()->trace( __VA_ARGS__)
	#define UNEYE_CORE_INFO(...)     ::Uneye::Log::GetLoggerMessage()->info( __VA_ARGS__)
	#define UNEYE_CORE_WARN(...)     ::Uneye::Log::GetLoggerMessage()->warn( __VA_ARGS__)
	#define UNEYE_CORE_ERROR(...)    ::Uneye::Log::GetLoggerMessage()->error( __VA_ARGS__)
	#define UNEYE_CORE_CRITICAL(...) ::Uneye::Log::GetLoggerMessage()->critical( __VA_ARGS__)

	// Client log macros
	#define UNEYE_TRACE(...)		 ::Uneye::Log::GetLoggerMessage()->trace( __VA_ARGS__)
	#define UNEYE_INFO(...)	      	 ::Uneye::Log::GetLoggerMessage()->info( __VA_ARGS__)
	#define UNEYE_WARN(...)			 ::Uneye::Log::GetLoggerMessage()->warn( __VA_ARGS__)
	#define UNEYE_ERROR(...)		 ::Uneye::Log::GetLoggerMessage()->error( __VA_ARGS__)
	#define UNEYE_CRITICAL(...)		 ::Uneye::Log::GetLoggerMessage()->critical( __VA_ARGS__)
#endif
