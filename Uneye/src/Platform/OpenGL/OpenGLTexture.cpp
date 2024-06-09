#include "uypch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>




namespace Uneye
{
	namespace Utils {

		static GLenum ImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB;
			case ImageFormat::RGBA8: return GL_RGBA;
			}

			UNEYE_CORE_ASSERT(true);
			return 0;
		}

		static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			}

			UNEYE_CORE_ASSERT(true);
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, Buffer data)
		: m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height), m_Data(&data.Data[0], &data.Data[0] + data.Size)
	{
		UNEYE_PROFILE_FUNCTION();

		m_InternalFormat = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
		m_Format = Utils::ImageFormatToGLDataFormat(m_Specification.Format);

		switch (m_Specification.Format)
		{
			case ImageFormat::RGB8: m_Channels = 3; break;
			case ImageFormat::RGBA8: m_Channels = 4; break;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		if (data)
			SetData(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		UNEYE_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(Buffer data)
	{
		UNEYE_PROFILE_FUNCTION();

		uint32_t bpp = m_Format == GL_RGBA ? 4 : 3;
		UNEYE_CORE_ASSERT(data.Size != (m_Width * m_Height * bpp), "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data.Data);
		//m_RendererHandle = glGetTextureHandleARB(m_RendererID);
	}

	void OpenGLTexture2D::ChangeSize(uint32_t p_width, uint32_t p_height)
	{
		// New Texture
		uint32_t newTexID;
		glCreateTextures(GL_TEXTURE_2D, 1, &newTexID);
		glTextureStorage2D(newTexID, 1, m_InternalFormat, p_width, p_height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		GLuint fboIDs[2] = { 0 };
		glGenFramebuffers(2, fboIDs);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIDs[0]);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, 
			GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RendererID, 0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboIDs[1]);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTexID, 0);

		glBlitFramebuffer(0, 0, m_Width, m_Height,
			0, 0, p_width, p_height,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glDeleteTextures(1, &m_RendererID);
		glDeleteFramebuffers(2, fboIDs);

		m_RendererID = newTexID;
		//m_Width = p_width;
		//m_Height = p_height;
	}

	void OpenGLTexture2D::Bind(uint32_t slot)
	{
		UNEYE_PROFILE_FUNCTION();

		//glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::Unbind()
	{
		UNEYE_PROFILE_FUNCTION();

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
