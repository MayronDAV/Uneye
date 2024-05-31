#pragma once

#include <filesystem>

#include "Uneye/Core/Base.h"
#include "Uneye/Renderer/Texture.h"


namespace Uneye
{
	struct MSDFData;

	class Font
	{
		public:
			Font(const std::filesystem::path& filepath);
			~Font();

			Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

		private:
			MSDFData* m_Data;
			Ref<Texture2D> m_AtlasTexture;
	};
}
