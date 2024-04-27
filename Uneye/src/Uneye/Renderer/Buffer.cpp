#include "uypch.h"
#include "Uneye/Renderer/Buffer.h"

#include "Uneye/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"


namespace Uneye
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
			{
				UNEYE_CORE_ASSERT(true, "RendererAPI::None is currently not support!");
				return nullptr;
			}
			case RendererAPI::OpenGL:
			{
				return new OpenGLVertexBuffer(vertices, size);
			}
		}
		
		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:
		{
			UNEYE_CORE_ASSERT(true, "RendererAPI::None is currently not support!");
			return nullptr;
		}
		case RendererAPI::OpenGL:
		{
			return new OpenGLIndexBuffer(indices, size);
		}
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}
}


