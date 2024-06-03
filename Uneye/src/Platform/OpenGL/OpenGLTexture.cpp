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
		: m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
	{
		UNEYE_PROFILE_FUNCTION();

		m_InternalFormat = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
		m_Format = Utils::ImageFormatToGLDataFormat(m_Specification.Format);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		if (data)
			SetData(data);

		glBindTexture(GL_TEXTURE_2D, 0);
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
