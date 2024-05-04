#pragma once

#include <string>
#include <glm/glm.hpp>



namespace Uneye
{

	class Shader
	{
		public:
			virtual ~Shader() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual int GetLocation(const std::string& name) const = 0;

			virtual void SetMat4(std::string name, const glm::mat4& value) = 0;
			virtual void SetMat3(std::string name, const glm::mat3& value) = 0;

			virtual void SetVec4(std::string name, float x, float y, float z, float w)  = 0;
			virtual void SetVec4(std::string name, const glm::vec4& value) = 0;
			virtual void SetVec4(std::string name, const float value[4]) = 0;
			virtual void SetVec3(std::string name, float x, float y, float z) = 0;
			virtual void SetVec3(std::string name, const glm::vec3& value) = 0;
			virtual void SetVec3(std::string name, const float value[3]) = 0;
			virtual void SetVec2(const std::string& name, glm::vec2 value) = 0;
			virtual void SetVec2(const std::string& name, const float value[2]) = 0;
			virtual void SetVec2(const std::string& name, float x, float y) = 0;

			virtual void SetBool(const std::string& name, bool value) = 0;
			virtual void SetInt(const std::string& name, int value) = 0;
			virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
			virtual void SetFloat(const std::string& name, float value) = 0;

			virtual void SetUiARB64(const std::string& name, uint64_t value) = 0;


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