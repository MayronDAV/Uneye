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

			void SetMat4(std::string name, const glm::mat4& value);

			void SetVec4(std::string name, const glm::vec4& value);
			void SetVec4(std::string name, const float value[4]);
			void SetVec3(std::string name, const glm::vec3& value);
			void SetVec3(std::string name, const float value[3]);

		private:
			uint32_t m_RendererID = 0;
	};
}