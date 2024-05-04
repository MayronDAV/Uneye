#include "uypch.h"
#include "SubTexture.h"




namespace Uneye
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture,
		const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{
		m_TexCoord[0] = { min.x, min.y };
		m_TexCoord[1] = { max.x, min.y };
		m_TexCoord[2] = { max.x, max.y };
		m_TexCoord[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromTexture(const Ref<Texture2D>& texture,
		const glm::vec2& coords, const glm::vec2& globalSize, const glm::vec2& spriteSize)
	{

		glm::vec2 min = {
			(coords.x * globalSize.x) / texture->GetWidth(),
			(coords.y * globalSize.y) / texture->GetHeight()
		};

		glm::vec2 max = {
			((coords.x + spriteSize.x) * globalSize.x) / texture->GetWidth(),
			((coords.y + spriteSize.y) * globalSize.y) / texture->GetHeight()
		};


		return CreateRef<SubTexture2D>(texture, min, max);
	}

}