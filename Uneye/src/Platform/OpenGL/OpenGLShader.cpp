#include "uypch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>



namespace Uneye
{
	OpenGLShader::OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath)
	{

		std::string vertexShaderSource;
		std::string fragmentShaderSource;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			std::stringstream vShaderStream, fShaderStream;
			// vertex
			vShaderFile.open(vertexPath);
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vertexShaderSource = vShaderStream.str();

			// fragment
			fShaderFile.open(fragmentPath);
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fragmentShaderSource = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			UNEYE_CORE_ASSERT(true, "File not succsfully read!");
		}

		GLuint vertex, fragment;
		int success;

		const char* vsSource = vertexShaderSource.c_str();
		const char* fsSource = fragmentShaderSource.c_str();

		// vertex
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vsSource, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		UNEYE_CORE_ASSERT(!success, "Vertex Shader compilation failed! ");

		// Fragment
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fsSource, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		UNEYE_CORE_ASSERT(!success, "Fragment Shader compilation failed! ");

		// Program setup
		// ----------------------------
		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertex);
		glAttachShader(m_RendererID, fragment);
		glLinkProgram(m_RendererID);

		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
		UNEYE_CORE_ASSERT(!success, "Shader link failed! ");

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}
	void OpenGLShader::SetMat4(std::string name, const glm::mat4& value)
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::SetVec4(std::string name, const glm::vec4& value)
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::SetVec4(std::string name, const float value[4])
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(loc, value[0], value[1], value[2], value[3]);
	}
	void OpenGLShader::SetVec3(std::string name, const glm::vec3& value)
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(loc, value.x, value.y, value.z);
	}
	void OpenGLShader::SetVec3(std::string name, const float value[3])
	{
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(loc, value[0], value[1], value[2]);
	}
}