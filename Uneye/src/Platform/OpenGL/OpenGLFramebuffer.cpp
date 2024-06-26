#include "uypch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"


#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


namespace Uneye
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return (multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static GLenum UneyeFBTextureInternalFormatToGL(FramebufferTextureFormat internalFormat)
		{
			switch (internalFormat)
			{
				case FramebufferTextureFormat::RGBA8:	return GL_RGBA8;
				case FramebufferTextureFormat::RGBA32F:	return GL_RGBA32F;
				case FramebufferTextureFormat::R32I:	return GL_R32I;
				case FramebufferTextureFormat::RG32UI:	return GL_RG32UI;
			}

			UNEYE_CORE_ASSERT(true);
			return 0;
		}

		static GLenum UneyeFBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:	return GL_RGBA;
				case FramebufferTextureFormat::RGBA32F:	return GL_RGBA;
				case FramebufferTextureFormat::R32I:	return GL_RED_INTEGER;
				case FramebufferTextureFormat::RG32UI:	return GL_RG_INTEGER;
			}

			UNEYE_CORE_ASSERT(true);
			return 0;
		}

		static GLenum GLToType(GLenum internalFormat)
		{
			switch (internalFormat)
			{
				case GL_RGBA32F:	  return GL_FLOAT;
				case GL_RGBA8:		  return GL_UNSIGNED_BYTE;
				case GL_R32I:		  return GL_INT;
				case GL_RG32UI:		  return GL_UNSIGNED_INT;

				default:			  return GL_UNSIGNED_BYTE;
			}
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format,
			uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_TRUE);
			}
			else
			{
				GLenum type = GLToType(internalFormat);			
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, 
				TextureTarget(multisampled), id, 0);

		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format,
			GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_TRUE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType,
				TextureTarget(multisampled), id, 0);

		}


		static bool isDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case Uneye::FramebufferTextureFormat::DEPTH24STENCIL8: 
					return true;
			}	

			return false;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		:m_Specification(spec)
	{
		UNEYE_PROFILE_FUNCTION();

		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::isDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecs.emplace_back(spec);
			else
				m_DepthAttachmentSpec = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		UNEYE_PROFILE_FUNCTION();

		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}


	void OpenGLFramebuffer::Invalidate()
	{
		UNEYE_PROFILE_FUNCTION();

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}
		

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;
		
		// Attachments
		if (m_ColorAttachmentSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);

				Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
					Utils::UneyeFBTextureInternalFormatToGL(m_ColorAttachmentSpecs[i].TextureFormat),
					Utils::UneyeFBTextureFormatToGL(m_ColorAttachmentSpecs[i].TextureFormat),
					m_Specification.Width, m_Specification.Height, i);

				Utils::BindTexture(multisample, 0);
			}
		}


		if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);

			switch (m_DepthAttachmentSpec.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
				{
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, 
						GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				}
			}
			Utils::BindTexture(multisample, 0);
		}

		if (m_ColorAttachments.size() > 1)
		{
			UNEYE_CORE_ASSERT(m_ColorAttachments.size() >= 32, "Only support 32 color attachment!");

			std::vector<GLenum> buffers;
			for (int i = 0; i < m_ColorAttachments.size(); i++)
			{
				buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
			}

			glDrawBuffers(m_ColorAttachments.size(), buffers.data());
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-buffer
			glDrawBuffer(GL_NONE);
		}

		UNEYE_CORE_ASSERT(
			glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, 
			"Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		UNEYE_PROFILE_FUNCTION();

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		UNEYE_PROFILE_FUNCTION();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		UNEYE_PROFILE_FUNCTION();

		if (width <= 0 || height <= 0 ||
			width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			UNEYE_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	void OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y, int w, int h, void* data)
	{
		UNEYE_CORE_ASSERT(attachmentIndex >= m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		auto& spec = m_ColorAttachmentSpecs[attachmentIndex];
		GLenum internalFormat = Utils::UneyeFBTextureInternalFormatToGL(spec.TextureFormat);
		GLenum format = Utils::UneyeFBTextureFormatToGL(spec.TextureFormat);

		glReadPixels(x, y, w, h, format, Utils::GLToType(internalFormat), data);
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, void* value)
	{
		UNEYE_CORE_ASSERT(attachmentIndex >= m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecs[attachmentIndex];
		GLenum internalFormat = Utils::UneyeFBTextureInternalFormatToGL(spec.TextureFormat);
		GLenum format = Utils::UneyeFBTextureFormatToGL(spec.TextureFormat);

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			format, Utils::GLToType(internalFormat), value);
	}

}
