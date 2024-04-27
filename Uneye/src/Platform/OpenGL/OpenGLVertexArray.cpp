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
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		UNEYE_CORE_ASSERT(!vertexBuffer->GetLayout().GetElements().size(), "vertexBuffer has NULL layout");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();


		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			//int loc = glGetAttribLocation("program", element.Name);
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				(element.Normalized) ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

}