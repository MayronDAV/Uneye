#include "uypch.h"
#include "Uneye/Core/Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Uneye {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	std::vector<LoggerMessage::LogMsg> LoggerMessage::s_LogMessages;

	std::shared_ptr<LoggerMessage> Log::s_LoggerMessage = CreateRef<LoggerMessage>();

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("UNEYE");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}

}
