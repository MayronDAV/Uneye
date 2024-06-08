#include "uypch.h"
#include "StringUtils.h"



namespace Uneye::Utils
{
	std::string BytesToString(uint64_t p_bytes)
	{
		constexpr uint64_t GB = 1024 * 1024 * 1024;
		constexpr uint64_t MB = 1024 * 1024;
		constexpr uint64_t KB = 1024;

		char buffer[32 + 1]{};

		if (p_bytes >= GB)
			snprintf(buffer, 32, "%.2f GB", (float)p_bytes / (float)GB);
		else if (p_bytes >= MB)
			snprintf(buffer, 32, "%.2f MB", (float)p_bytes / (float)MB);
		else if (p_bytes >= KB)
			snprintf(buffer, 32, "%.2f KB", (float)p_bytes / (float)KB);
		else
			snprintf(buffer, 32, "%.2f bytes", (float)p_bytes);

		return std::string(buffer);
	}
}
