#include "uypch.h"
#include "SubTexture.h"

#include "Uneye/Asset/AssetManager.h"


namespace Uneye
{
	SubTexture2D::SubTexture2D(const AssetHandle& p_texturehandle,
		const glm::vec2& p_min, const glm::vec2& p_max)
		: m_TextureHandle(p_texturehandle)
	{
		m_TexCoord[0] = { p_min.x, p_min.y };
		m_TexCoord[1] = { p_max.x, p_min.y };
		m_TexCoord[2] = { p_max.x, p_max.y };
		m_TexCoord[3] = { p_min.x, p_max.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromTexture(const AssetHandle& p_texturehandle,
		const glm::vec2& p_tilesize, const glm::vec2& p_tilecoord, const glm::vec2& p_spritesize)
	{
		if (!AssetManager::IsAssetHandleValid(p_texturehandle))
		{
			UNEYE_CORE_ERROR("This handle is invalid!");
			return nullptr;
		}

		if (!AssetManager::IsAssetLoaded(p_texturehandle))
		{
			UNEYE_CORE_ERROR("Make sure to load the asset before passing the handle.");
			return nullptr;
		}

		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(p_texturehandle);

		glm::vec2 min = {
			(p_tilecoord.x * p_tilesize.x) / texture->GetWidth(),
			(p_tilecoord.y * p_tilesize.y) / texture->GetHeight()
		};

		glm::vec2 max = {
			((p_tilecoord.x + p_spritesize.x) * p_tilesize.x) / texture->GetWidth(),
			((p_tilecoord.y + p_spritesize.y) * p_tilesize.y) / texture->GetHeight()
		};

		return CreateRef<SubTexture2D>(p_texturehandle, min, max);
	}

}
