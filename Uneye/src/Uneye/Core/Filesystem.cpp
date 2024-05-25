#include "uypch.h"
#include "FileSystem.h"

#include <fstream>

namespace Uneye
{

	Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			UNEYE_CORE_ERROR("Failed to open the file {}", filepath.filename().string());
			return {};
		}

		stream.seekg(0, std::ios::end);
		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint64_t size = end - stream.tellg();

		if (size == 0)
		{
			UNEYE_CORE_ERROR("File {} is empty", filepath.filename().string());
			return {};
		}

		Buffer buffer(size);
		stream.read(buffer.As<char>(), size);
		stream.close();
		return buffer;
	}

	std::stringstream FileSystem::ReadFileString(const std::filesystem::path& filepath)
	{
		std::stringstream result;
		std::ifstream stream(filepath, std::ios::in | std::ios::binary);

		if (!stream)
		{
			UNEYE_CORE_ERROR("Failed to open the file {}", filepath.filename().string());
			return {};
		}

		stream.seekg(0, std::ios::end);
		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint64_t size = end - stream.tellg();

		if (size == 0)
		{
			UNEYE_CORE_ERROR("File {} is empty", filepath.filename().string());
			return {};
		}

		result << stream.rdbuf();
		stream.close();

		stream.close();
		return result;
	}

}
