#include "uypch.h"
#include "SubTexture.h"

#include "Uneye/Asset/AssetManager.h"


namespace Uneye
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& p_texture,
		const glm::vec2& p_min, const glm::vec2& p_max)
		: m_Texture(p_texture)
	{
		m_TexCoord[0] = { p_min.x, p_min.y };
		m_TexCoord[1] = { p_max.x, p_min.y };
		m_TexCoord[2] = { p_max.x, p_max.y };
		m_TexCoord[3] = { p_min.x, p_max.y };
	}


	Ref<SubTexture2D> SubTexture2D::CreateFromTexture(const Ref<Texture2D>& p_texture,
		const glm::vec2& p_tilesize, const glm::vec2& p_tilecoord, const glm::vec2& p_spritesize)
	{
		glm::vec2 min = {
			(p_tilecoord.x * p_tilesize.x) / p_texture->GetWidth(),
			(p_tilecoord.y * p_tilesize.y) / p_texture->GetHeight()
		};

		glm::vec2 max = {
			((p_tilecoord.x + p_spritesize.x) * p_tilesize.x) / p_texture->GetWidth(),
			((p_tilecoord.y + p_spritesize.y) * p_tilesize.y) / p_texture->GetHeight()
		};

		return CreateRef<SubTexture2D>(p_texture, min, max);
	}

}
