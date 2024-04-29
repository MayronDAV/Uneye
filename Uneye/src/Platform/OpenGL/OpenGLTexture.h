#pragma once

#include <string>
#include "Uneye/Renderer/Texture.h"



namespace Uneye
{


	class OpenGLTexture2D : public Texture2D
	{
		public:
			OpenGLTexture2D(const std::string& path);
			virtual ~OpenGLTexture2D();

			virtual uint32_t GetWidth() const override { return m_Width;  }
			virtual uint32_t GetHeight() const override { return m_Height; }

			virtual void Bind(uint32_t slot) override;
			virtual void Unbind() override;

		private:
			std::string m_Path			= NULL;
			uint32_t m_Width			= NULL;
			uint32_t m_Height			= NULL;
			uint32_t m_RendererID		= NULL;
			uint64_t m_RendererHandle	= NULL;
			// for bind handle
			bool m_Used = false;
	};
}