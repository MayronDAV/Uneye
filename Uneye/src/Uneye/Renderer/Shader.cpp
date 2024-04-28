#include "uypch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"


namespace Uneye
{
	Shader* Shader::Create(const std::string& vertexPath, const std::string& fragmentPath)
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
				return new OpenGLShader(vertexPath, fragmentPath);
			}
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}
}