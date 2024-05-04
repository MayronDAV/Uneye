#pragma once

#include <glm/glm.hpp>

#include "Texture.h"



namespace Uneye
{
	class SubTexture2D
	{
		public:
			SubTexture2D(const Ref<Texture2D>& texture, 
				const glm::vec2& min, const glm::vec2& max);

			const Ref<Texture2D> GetTexture() const { return m_Texture;  }
			const glm::vec2* GetTexCoords() const { return m_TexCoord; }

			static Ref<SubTexture2D> CreateFromTexture(const Ref<Texture2D>& texture,
				const glm::vec2& coords, const glm::vec2& globalSize, const glm::vec2& spriteSize = {1, 1});

		private:
			Ref<Texture2D> m_Texture;

			glm::vec2 m_TexCoord[4];
	};
};