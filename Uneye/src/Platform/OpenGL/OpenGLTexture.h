#pragma once

#include <string>
#include "Uneye/Renderer/Texture.h"



namespace Uneye
{


	class OpenGLTexture2D : public Texture2D
	{
		public:
			OpenGLTexture2D(uint32_t width, uint32_t height);
			OpenGLTexture2D(const std::string& path);
			virtual ~OpenGLTexture2D();

			virtual void SetData(void* data, uint32_t size) override;

			virtual uint32_t GetWidth() const override { return m_Width;  }
			virtual uint32_t GetHeight() const override { return m_Height; }

			virtual void Bind(uint32_t slot) override;
			virtual void Unbind() override;

		private:
			std::string m_Path			= " ";
			uint32_t m_Width			= NULL;
			uint32_t m_Height			= NULL;
			uint32_t m_RendererID		= NULL;
			uint64_t m_RendererHandle	= NULL;

			uint32_t m_InternalFormat, m_Format;

			// for bind handle
			bool m_Used = false;
	};
}