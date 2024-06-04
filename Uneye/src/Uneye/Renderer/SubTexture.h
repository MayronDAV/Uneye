#pragma once

#include <glm/glm.hpp>

#include "Uneye/Asset/Asset.h"
#include "Texture.h"



namespace Uneye
{
	class SubTexture2D
	{
		public:
			SubTexture2D(const Ref<Texture2D>& p_texture,
				const glm::vec2& p_min, const glm::vec2& p_max);

			const Ref<Texture2D>& GetTexture() const { return m_Texture;  }
			const glm::vec2* GetTexCoords() const { return m_TexCoord; }

			static Ref<SubTexture2D> CreateFromTexture(const Ref<Texture2D>& p_texture,
				const glm::vec2& p_tilesize, const glm::vec2& p_tilecoord, const glm::vec2& p_spriteSize = {1, 1});

		private:
			Ref<Texture2D> m_Texture;

			glm::vec2 m_TexCoord[4];
	};
};
