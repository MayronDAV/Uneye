#include "uypch.h"
#include "Texture.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"



namespace Uneye
{
	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer data)
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
				return CreateRef<OpenGLTexture2D>(specification, data);
			}
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}
}
