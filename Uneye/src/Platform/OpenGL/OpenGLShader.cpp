#include "uypch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>



namespace Uneye
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		UNEYE_PROFILE_FUNCTION();

		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		UNEYE_CORE_ASSERT(true, "Unknown shader type");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	{
		UNEYE_PROFILE_FUNCTION();

		std::string vertexShaderSource;
		std::string fragmentShaderSource;
		std::ifstream vShaderFile{vertexPath, std::ios::in | std::ios::binary };
		if (vShaderFile)
		{
			UNEYE_PROFILE_SCOPE("Read vertex shader file");

			std::stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vertexShaderSource = vShaderStream.str();
		}
		else
		{
			UNEYE_CORE_ASSERT(true, "Vertex shader file not succsfully read!");
		}
		std::ifstream fShaderFile{ fragmentPath, std::ios::in | std::ios::binary };
		if (fShaderFile)
		{
			UNEYE_PROFILE_SCOPE("Read fragment shader file");

			std::stringstream fShaderStream;
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fragmentShaderSource = fShaderStream.str();
		}
		else
		{
			UNEYE_CORE_ASSERT(true, "Fragment shader file not succsfully read!");
		}

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexShaderSource;
		sources[GL_FRAGMENT_SHADER] = fragmentShaderSource;
		Compile(sources);
	}

	OpenGLShader::OpenGLShader(const std::string& shaderPath)
	{
		UNEYE_PROFILE_FUNCTION();

		std::string source = ReadFile(shaderPath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extract name from shaderPath
		auto lastSlash = shaderPath.find_last_of("/\\");
		lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
		auto lastDot = shaderPath.rfind('.');
		lastDot = (lastDot == std::string::npos) ? shaderPath.size() : lastDot;

		m_Name = shaderPath.substr(lastSlash, lastDot - lastSlash);
	}

	OpenGLShader::~OpenGLShader()
	{
		UNEYE_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		std::string result;
		std::ifstream shaderFile{ filepath, std::ios::in | std::ios::binary };

		if (shaderFile)
		{
			UNEYE_PROFILE_SCOPE("Read one shader file");

			shaderFile.seekg(0, std::ios::end);
			result.resize(shaderFile.tellg());
			shaderFile.seekg(0, std::ios::beg);

			shaderFile.read(&result[0], result.size());
			shaderFile.close();
		}
		else
		{
			UNEYE_CORE_ERROR("Could not open file '{0}' ", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		UNEYE_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "@type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			UNEYE_CORE_ASSERT(eol == std::string::npos, "Syntax error");

			size_t begin = source.find_first_not_of(" \t", pos + typeTokenLength);
			size_t end = source.find_last_not_of(" \t", eol);
			std::string type = source.substr(begin, end - begin + 1);
			size_t typeEnd = type.find_first_of(" \r\n");
			if (typeEnd != std::string::npos) {
				type.erase(typeEnd);
			}
			UNEYE_CORE_ASSERT(!ShaderTypeFromString(type), "Invalid shader type specification");

			// Encontrar a pr�xima linha de shader
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(
				nextLinePos,
				pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		UNEYE_PROFILE_FUNCTION();

		int success = false;
		GLuint program = glCreateProgram();

		UNEYE_CORE_ASSERT(!(shaderSources.size() <= 2), "An only support 2 shaders");
		std::array<GLuint, 2> glShaderIDS;
		int glShaderIDindex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			const char* sourceChar = source.c_str();

			GLuint shader = glCreateShader(type);
			glShaderSource(shader, 1, &sourceChar, NULL);
			glCompileShader(shader);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (success == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				UNEYE_CORE_ERROR("{0}", infoLog.data());
				UNEYE_CORE_ASSERT(true, "Shader compilation failed! ");
				break;
			}
			else
			{
				glAttachShader(program, shader);
				glShaderIDS[glShaderIDindex++] = shader;
			}

		}

		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDS)
				glDeleteShader(id);

			UNEYE_CORE_ERROR("{0}", infoLog.data());
			UNEYE_CORE_ASSERT(true, "Shader link failed!");
		}

		m_RendererID = program;

		for (auto id : glShaderIDS)
			glDetachShader(program, id);
	}



	void OpenGLShader::Bind() const
	{
		UNEYE_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		UNEYE_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	int OpenGLShader::GetLocation(const std::string& name) const
	{
		return glGetUniformLocation(m_RendererID, name.c_str());
	}

	void OpenGLShader::SetMat4(std::string name, const glm::mat4& value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadMat4(name, value);
	}

	void OpenGLShader::SetMat3(std::string name, const glm::mat3& value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadMat3(name, value);
	}

	void OpenGLShader::SetVec4(std::string name, float x, float y, float z, float w)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec4(name, x, y, z, w);
	}

	void OpenGLShader::SetVec4(std::string name, const glm::vec4& value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec4(name, value);
	}

	void OpenGLShader::SetVec4(std::string name, const float value[4])
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec4(name, value);
	}

	void OpenGLShader::SetVec3(std::string name, float x, float y, float z)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec3(name, x, y, z);
	}

	void OpenGLShader::SetVec3(std::string name, const glm::vec3& value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec3(name, value);
	}

	void OpenGLShader::SetVec3(std::string name, const float value[3])
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec3(name, value);
	}

	void OpenGLShader::SetVec2(const std::string& name, glm::vec2 value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec2(name, value);
	}

	void OpenGLShader::SetVec2(const std::string& name, const float value[2])
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec2(name, value);
	}

	void OpenGLShader::SetVec2(const std::string& name, float x, float y)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadVec2(name, x, y);
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadBool(name, value);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadFloat(name, value);
	}

	void OpenGLShader::SetUiARB64(const std::string& name, uint64_t value)
	{
		UNEYE_PROFILE_FUNCTION();

		UploadUiARB64(name, value);
	}



	void OpenGLShader::UploadMat4(std::string name, const glm::mat4& value) const
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::UploadMat3(std::string name, const glm::mat3& value) const
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}



	void OpenGLShader::UploadVec4(std::string name, float x, float y, float z, float w) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(loc, x, y, z, w);
	}
	void OpenGLShader::UploadVec4(std::string name, const glm::vec4& value) const
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadVec4(std::string name, const float value[4]) const
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(loc, value[0], value[1], value[2], value[3]);
	}
	void OpenGLShader::UploadVec3(std::string name, float x, float y, float z) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(loc, x, y, z);
	}
	void OpenGLShader::UploadVec3(std::string name, const glm::vec3& value) const
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(loc, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadVec3(std::string name, const float value[3]) const
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(loc, value[0], value[1], value[2]);
	}
	void OpenGLShader::UploadVec2(const std::string& name, glm::vec2 value) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(loc, value.x, value.y);
	}

	void OpenGLShader::UploadVec2(const std::string& name, const float value[2]) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(loc, value[0], value[1]);
	}	

	void OpenGLShader::UploadVec2(const std::string& name, float x, float y) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(loc, x, y);
	}




	void OpenGLShader::UploadBool(const std::string& name, bool value) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(loc, value);
	}

	void OpenGLShader::UploadInt(const std::string& name, int value) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(loc, (int)value);
	}

	void OpenGLShader::UploadIntArray(const std::string& name, int* values, uint32_t count) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(loc, count, values);
	}

	void OpenGLShader::UploadFloat(const std::string& name, float value) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(loc, value);
	}

	void OpenGLShader::UploadUiARB64(const std::string& name, uint64_t value) const
	{
		int loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformHandleui64ARB(loc, value);
	}



}