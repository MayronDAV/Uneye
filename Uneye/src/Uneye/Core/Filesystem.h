#pragma once

#include "Uneye/Core/Buffer.h"
#include <filesystem>

namespace Uneye
{

	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
		// Maybe change this to take a better perfomance
		static std::stringstream ReadFileString(const std::filesystem::path& filepath);
	};

}
