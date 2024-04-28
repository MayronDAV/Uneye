#pragma once

#include "Uneye/Renderer/Shader.h"
#include <string>
#include <glm/glm.hpp>

namespace Uneye
{
	class OpenGLShader : public Shader
	{
		public:
			OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath);
			virtual ~OpenGLShader();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			void SetMat4(std::string name, const glm::mat4& value)	const;
			void SetMat3(std::string name, const glm::mat3& value)	const;

			void SetVec4(std::string name, float x, float y, float z, float w) const;
			void SetVec4(std::string name, const glm::vec4& value)	const;
			void SetVec4(std::string name, const float value[4])	const;
			void SetVec3(std::string name, float x, float y, float z) const;
			void SetVec3(std::string name, const glm::vec3& value)	const;
			void SetVec3(std::string name, const float value[3])	const;
			void SetVec2(const std::string& name, glm::vec2 value)	const;
			void SetVec2(const std::string& name, const float value[2])	const;
			void SetVec2(const std::string& name, float x, float y) const;

            void SetBool(const std::string& name, bool value)		const;
            void SetInt(const std::string& name, int value)			const;
			void SetFloat(const std::string& name, float value)		const;

			void SetUiARB64(const std::string& name, uint64_t value)const;

		private:
			uint32_t m_RendererID = 0;
	};
}