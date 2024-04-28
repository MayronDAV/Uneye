#include "uypch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Uneye
{
	void OpenGLRendererAPI::Clear(const glm::vec4& color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffers()->GetCount(),
			GL_UNSIGNED_INT, nullptr);
	}
}