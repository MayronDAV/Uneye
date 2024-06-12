#include "uypch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Uneye
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Uneye::ShaderDataType::Float:		return GL_FLOAT;
			case Uneye::ShaderDataType::Float2:		return GL_FLOAT;
			case Uneye::ShaderDataType::Float3:		return GL_FLOAT;
			case Uneye::ShaderDataType::Float4:		return GL_FLOAT;
			case Uneye::ShaderDataType::Mat3:		return GL_FLOAT;
			case Uneye::ShaderDataType::Mat4:		return GL_FLOAT;
			case Uneye::ShaderDataType::UInt32:		return GL_UNSIGNED_INT;
			case Uneye::ShaderDataType::UInt64:		return GL_UNSIGNED_INT64_ARB;
			case Uneye::ShaderDataType::Int:		return GL_INT;
			case Uneye::ShaderDataType::Int2:		return GL_INT;
			case Uneye::ShaderDataType::Int3:		return GL_INT;
			case Uneye::ShaderDataType::Int4:		return GL_INT;
			case Uneye::ShaderDataType::Bool:		return GL_BOOL;
		}

		UNEYE_CORE_ASSERT(true, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		UNEYE_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		UNEYE_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		UNEYE_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::Unbind() const
	{
		UNEYE_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		UNEYE_PROFILE_FUNCTION();

		UNEYE_CORE_ASSERT(!vertexBuffer->GetLayout().GetElements().size(), "vertexBuffer has NULL layout");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();


		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(const uint64_t)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::UInt32:
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(const uint64_t)element.Offset);
				m_VertexBufferIndex++;
				break;
			case ShaderDataType::UInt64:
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribL1ui64ARB(m_VertexBufferIndex, (GLuint64EXT)element.Offset);
				break;

			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribIPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)(const uint64_t)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i));
					glVertexAttribDivisor(m_VertexBufferIndex, 1);
					m_VertexBufferIndex++;
				}
				break;
			}
			default:
				UNEYE_CORE_ASSERT(true, "Unknown ShaderDataType!");
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		UNEYE_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

}
