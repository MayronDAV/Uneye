#include "uypch.h"
#include "Uneye/Renderer/VertexArray.h"

#include "Uneye/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Uneye/Core/Base.h"


namespace Uneye
{
	Ref<VertexArray> VertexArray::Create()
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
				return CreateRef<OpenGLVertexArray>();
			}
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}
}