#include "uypch.h"
#include "CompressionManager.h"

#include <zlib.h>


namespace Uneye
{

	std::vector<unsigned char> CompressionManager::Compress(const std::vector<unsigned char>& p_data)
	{
		uLongf compressedSize = compressBound(p_data.size());
		std::vector<unsigned char> compressedData(compressedSize);

		int result = compress(compressedData.data(), &compressedSize, p_data.data(), p_data.size());
		if (result != Z_OK)
		{
			UNEYE_CORE_ERROR("Compression failed!");
		}

		compressedData.resize(compressedSize);
		return compressedData;
	}

	std::vector<unsigned char> CompressionManager::Decompress(const std::vector<unsigned char>& p_compressedData, size_t p_size)
	{
		std::vector<unsigned char> decompressedData(p_size);

		uLongf decompressedSize = p_size;
		int result = uncompress(decompressedData.data(), &decompressedSize, p_compressedData.data(), p_compressedData.size());
		if (result != Z_OK)
		{
			UNEYE_CORE_ERROR("Decompression failed!");
		}

		return decompressedData;
	}

}
