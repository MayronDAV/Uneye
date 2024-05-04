#include "uypch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>



namespace Uneye
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height)
	{
		UNEYE_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_Format = GL_RGBA;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		UNEYE_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);

		unsigned char* data;
		{
			UNEYE_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		UNEYE_CORE_ASSERT(!data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		m_InternalFormat = internalFormat;
		m_Format = dataFormat;


		UNEYE_CORE_ASSERT(!internalFormat & !dataFormat, "Format not supported!");

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);


		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		UNEYE_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		UNEYE_PROFILE_FUNCTION();

		//UNEYE_CORE_ASSERT(size != (m_Width * m_Height * 3), "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data);
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