#pragma once

#include "Uneye/Renderer/Framebuffer.h"

#include <glm/glm.hpp>



namespace Uneye
{
	class OpenGLFramebuffer : public Framebuffer
	{
		public:
			OpenGLFramebuffer(const FramebufferSpecification& spec);
			virtual ~OpenGLFramebuffer();

			virtual void Invalidate() override;

			virtual void Bind() override;
			virtual void Unbind() override;

			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void ReadPixel(uint32_t attachmentIndex, int x, int y, int w, int h, void* data) override;
			virtual void ClearAttachment(uint32_t attachmentIndex, void* value) override;

			virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const override {
				UNEYE_CORE_ASSERT(index >= m_ColorAttachments.size(), "Index must be less than size of m_ColorAttachments");
				return m_ColorAttachments[index];
			}

			virtual const FramebufferSpecification& GetSpecification() const override {
				return m_Specification;
			};

		private:
			uint32_t m_RendererID = 0;
			FramebufferSpecification m_Specification;

			std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
			FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureFormat::None;

			std::vector<uint32_t> m_ColorAttachments;
			uint32_t m_DepthAttachment = 0;
	};




}
