#include "uypch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>



namespace Uneye
{
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         UNEYE_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       UNEYE_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          UNEYE_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: UNEYE_CORE_TRACE(message); return;
		}

		UNEYE_CORE_ASSERT(true, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		UNEYE_PROFILE_FUNCTION();

	#ifdef UNEYE_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

	#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::ClearColor(const glm::vec4& color) const
	{
		UNEYE_PROFILE_FUNCTION();

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::ClearDepth() const
	{
		UNEYE_PROFILE_FUNCTION();

		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		UNEYE_PROFILE_FUNCTION();

		vertexArray->Bind();
		uint32_t count = (indexCount == 0) ? vertexArray->GetIndexBuffers()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		UNEYE_PROFILE_FUNCTION();

		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}
