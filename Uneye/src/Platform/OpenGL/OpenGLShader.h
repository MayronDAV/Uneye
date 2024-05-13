#pragma once

#include "Uneye/Renderer/Shader.h"
#include <string>
#include <glm/glm.hpp>



namespace Uneye
{


	class OpenGLShader : public Shader
	{
		public:
			OpenGLShader(const std::string& filePath);
			virtual ~OpenGLShader();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			virtual int GetLocation(const std::string& name) const override;

			virtual void SetMat4(std::string name, const glm::mat4& value) override;
			virtual void SetMat3(std::string name, const glm::mat3& value) override;

			virtual void SetVec4(std::string name, float x, float y, float z, float w) override;
			virtual void SetVec4(std::string name, const glm::vec4& value) override;
			virtual void SetVec4(std::string name, const float value[4]) override;
			virtual void SetVec3(std::string name, float x, float y, float z) override;
			virtual void SetVec3(std::string name, const glm::vec3& value) override;
			virtual void SetVec3(std::string name, const float value[3]) override;
			virtual void SetVec2(const std::string& name, glm::vec2 value) override;
			virtual void SetVec2(const std::string& name, const float value[2]) override;
			virtual void SetVec2(const std::string& name, float x, float y) override;

			virtual void SetBool(const std::string& name, bool value) override;
			virtual void SetInt(const std::string& name, int value) override;
			virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
			virtual void SetFloat(const std::string& name, float value) override;

			virtual void SetUiARB64(const std::string& name, uint64_t value) override;




			void UploadMat4(std::string name, const glm::mat4& value) const;
			void UploadMat3(std::string name, const glm::mat3& value) const;

			void UploadVec4(std::string name, float x, float y, float z, float w) const;
			void UploadVec4(std::string name, const glm::vec4& value) const;
			void UploadVec4(std::string name, const float value[4])	const;
			void UploadVec3(std::string name, float x, float y, float z) const;
			void UploadVec3(std::string name, const glm::vec3& value) const;
			void UploadVec3(std::string name, const float value[3])	const;
			void UploadVec2(const std::string& name, glm::vec2 value) const;
			void UploadVec2(const std::string& name, const float value[2]) const;
			void UploadVec2(const std::string& name, float x, float y) const;

            void UploadBool(const std::string& name, bool value)	const;
            void UploadInt(const std::string& name, int value)		const;
			void UploadIntArray(const std::string& name, int* values, uint32_t count) const;
			void UploadFloat(const std::string& name, float value)	const;

			void UploadUiARB64(const std::string& name, uint64_t value) const;

			virtual const std::string& GetName() const override { return m_Name; };

		private:
			std::string ReadFile(const std::string& filepath);
			std::unordered_map<unsigned int, std::string> PreProcess(const std::string& source);

			void CompileOrGetVulkanBinaries(const std::unordered_map<unsigned int, std::string>& shaderSources);
			void CompileOrGetOpenGLBinaries();
			void CreateProgram();
			void Reflect(unsigned int stage, const std::vector<uint32_t>& shaderData);

		private:
			uint32_t m_RendererID = 0;
			std::string m_FilePath;
			std::string m_Name;

			std::unordered_map<unsigned int, std::vector<uint32_t>> m_VulkanSPIRV;
			std::unordered_map<unsigned int, std::vector<uint32_t>> m_OpenGLSPIRV;

			std::unordered_map<unsigned int, std::string> m_OpenGLSourceCode;
	};
}
