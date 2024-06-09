#pragma once

#include <string>
#include "Uneye/Renderer/Texture.h"



namespace Uneye
{


	class OpenGLTexture2D : public Texture2D
	{
		public:
			OpenGLTexture2D(const TextureSpecification& specification, Buffer data = Buffer());
			virtual ~OpenGLTexture2D();

			virtual void SetData(Buffer data) override;
			virtual uint32_t GetRendererID() override { return m_RendererID; }

			virtual uint64_t GetEstimatedSize() const override { return m_Width * m_Height * 4; }

			virtual void ChangeSize(uint32_t p_width, uint32_t p_height) override;

			virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }
			virtual std::vector<unsigned char>& GetData() override { return m_Data; }

			virtual uint32_t GetWidth() const override { return m_Width;  }
			virtual uint32_t GetHeight() const override { return m_Height; }
			virtual uint32_t GetChannels() const override { return m_Channels; }

			virtual void Bind(uint32_t slot) override;
			virtual void Unbind() override;

			virtual bool operator== (const Texture& other) const override
			{
				return m_RendererID == ((OpenGLTexture2D&)other).GetRendererID();
			}

		private:
			std::vector<unsigned char> m_Data;

			TextureSpecification m_Specification;

			uint32_t m_Width			= NULL;
			uint32_t m_Height			= NULL;
			uint32_t m_Channels			= NULL;
			uint32_t m_RendererID		= NULL;

			uint32_t m_InternalFormat, m_Format;

			// for bind handle
			bool m_Used = false;
	};
}
