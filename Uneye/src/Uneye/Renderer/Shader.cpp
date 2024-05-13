#include "uypch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Uneye/Core/Base.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace Uneye
{
	//////////////////////////////////////////////////////////////////////
	// Shader ////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	Ref<Shader> Shader::Create(const std::string& shaderPath)
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
				return CreateRef<OpenGLShader>(shaderPath);
			}
		}

		UNEYE_CORE_ASSERT(true, "Unknown RendererAPI!");
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////
	// Shader Library ////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////


	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		UNEYE_CORE_ASSERT(Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string name)
	{
		UNEYE_CORE_ASSERT(!Exists(name), "Shader already exists!");
		return m_Shaders[name];
	}
	bool ShaderLibrary::Exists(const std::string name) const
	{
		return (m_Shaders.find(name) != m_Shaders.end());
	}
}
