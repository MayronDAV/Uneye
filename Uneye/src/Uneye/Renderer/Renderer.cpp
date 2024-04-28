#include "uypch.h"
#include "Uneye/Renderer/Renderer.h"

namespace Uneye
{
	void Renderer::BeginScene()
	{

	}
	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}