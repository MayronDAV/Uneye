#pragma once

#include <vector>

namespace Uneye
{
	class CompressionManager
	{
		public:
			static std::vector<unsigned char> Compress(const std::vector<unsigned char>& p_data);
			static std::vector<unsigned char> Decompress(const std::vector<unsigned char>& p_compressedData, size_t p_size);
	};

}
