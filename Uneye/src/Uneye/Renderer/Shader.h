#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Uneye
{
	class Shader
	{
		public:
			Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
			~Shader();

			void Bind() const;
			void Unbind() const;

			void SetMat4(std::string name, const glm::mat4& value);
			void SetVec4(std::string name, const glm::vec4& value);
			void SetVec3(std::string name, const glm::vec3& value);

		private:
			uint32_t m_RendererID = 0;
	};
}