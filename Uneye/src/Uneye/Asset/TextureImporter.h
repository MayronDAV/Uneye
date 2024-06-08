#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Uneye/Renderer/Texture.h"
#include <Uneye/Renderer/SubTexture.h>

namespace Uneye 
{

	class TextureImporter
	{
		public:
			// AssetMetadata filepath is relative to project asset directory
			static Ref<Texture2D> ImportTexture2D(AssetHandle p_handle, const AssetMetadata& p_metadata);

			// Reads file directly from filesystem
			// (i.e. path has to be relative / absolute to working directory)
			static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& p_path);

			static Ref<Texture2D> LoadTexture2D(const Buffer& p_data) {};
	};
}

