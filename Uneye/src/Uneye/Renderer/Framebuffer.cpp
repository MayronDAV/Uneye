#include "uypch.h"
#include "Uneye/Renderer/Framebuffer.h"

#include "Uneye/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Uneye/Core/Base.h"


namespace Uneye
{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{

		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			{
				UNEYE_CORE_ASSERT(true, "RendererAPI::None is currently not support!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return CreateRef<OpenGLFramebuffer>(spec);
			}
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}

}