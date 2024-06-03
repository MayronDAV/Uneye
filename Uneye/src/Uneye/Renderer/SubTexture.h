#pragma once

#include <glm/glm.hpp>

#include "Uneye/Asset/Asset.h"
#include "Texture.h"



namespace Uneye
{
	class SubTexture2D
	{
		public:
			SubTexture2D(const AssetHandle& p_texturehandle, 
				const glm::vec2& p_min, const glm::vec2& p_max);

			const AssetHandle GetTextureHandle() const { return m_TextureHandle;  }
			const glm::vec2* GetTexCoords() const { return m_TexCoord; }

			static Ref<SubTexture2D> CreateFromTexture(const AssetHandle& p_texturehandle,
				const glm::vec2& p_tilesize, const glm::vec2& p_tilecoord, const glm::vec2& p_spriteSize = {1, 1});

		private:
			AssetHandle m_TextureHandle;

			glm::vec2 m_TexCoord[4];
	};
};
