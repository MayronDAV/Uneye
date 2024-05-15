#include "uypch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Uneye/Core/Timer.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>



namespace Uneye
{
	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			UNEYE_CORE_ASSERT(true, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
				case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			UNEYE_CORE_ASSERT(true, "");
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			UNEYE_CORE_ASSERT(true, "");
			return nullptr;
		}

		static void CreateDirectoryIfNeeded(const std::string& path)
		{
			if (!std::filesystem::exists(path))
				std::filesystem::create_directories(path);
		}

		static const char* GetCacheDirectory()
		{
			CreateDirectoryIfNeeded("assets");
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			CreateDirectoryIfNeeded(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
				case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			UNEYE_CORE_ASSERT(true, "");
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
				case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			UNEYE_CORE_ASSERT(true, "");
			return "";
		}

	}

	OpenGLShader::OpenGLShader(const std::string& shaderPath)
		: m_FilePath(shaderPath)
	{
		UNEYE_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(shaderPath);
		auto shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			UNEYE_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

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
			UNEYE_CORE_ASSERT(!Utils::ShaderTypeFromString(type), "Invalid shader type specification");

			// Encontrar a pr�xima linha de shader
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[Utils::ShaderTypeFromString(type)] = source.substr(
				nextLinePos,
				pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					UNEYE_CORE_ERROR(module.GetErrorMessage());
					UNEYE_CORE_ASSERT(true, "");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					UNEYE_CORE_ERROR(module.GetErrorMessage());
					UNEYE_CORE_ASSERT(true, "");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);

			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			UNEYE_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

			for (auto id : shaderIDs)
				glDeleteShader(id);

		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		UNEYE_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		UNEYE_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		UNEYE_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		UNEYE_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			UNEYE_CORE_TRACE("  {0}", resource.name);
			UNEYE_CORE_TRACE("    Size = {0}", bufferSize);
			UNEYE_CORE_TRACE("    Binding = {0}", binding);
			UNEYE_CORE_TRACE("    Members = {0}", memberCount);
		}
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
