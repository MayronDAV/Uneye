#pragma once

#include <string>



namespace Uneye
{

	class Shader
	{
		public:
			virtual ~Shader() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual const std::string& GetName() const = 0;

			// Create a shader with vertex and fragment shader in different files
			static Ref<Shader> Create(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
			// Create a shader with vertex and fragment shader in one file
			static Ref<Shader> Create(const std::string& shaderPath);

	};



	class ShaderLibrary
	{
		public:
			void Add(const std::string& name, const Ref<Shader>& shader);
			void Add(const Ref<Shader>& shader);
			Ref<Shader> Load(const std::string& filepath);
			Ref<Shader> Load(const std::string& name, const std::string& filepath);

			Ref<Shader> Get(const std::string name);

		private:
			bool Exists(const std::string name) const;

		private:
			std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}