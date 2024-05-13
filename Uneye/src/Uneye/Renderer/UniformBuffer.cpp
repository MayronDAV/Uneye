#include "uypch.h"
#include "UniformBuffer.h"

#include "Uneye/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Uneye {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    UNEYE_CORE_ASSERT(true, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}

}
