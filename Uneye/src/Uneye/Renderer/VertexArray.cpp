#include "uypch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace Uneye
{
	VertexArray* VertexArray::Create()
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
				return new OpenGLVertexArray();
			}
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}
}