#pragma once

#include "RendererAPI.h"


namespace Uneye
{
	class RenderCommand
	{
		public:
			static void Init()
			{
				s_RendererAPI->Init();
			}

			static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
			{
				s_RendererAPI->SetViewport(x, y, width, height);
			}

			static void ClearColor(const glm::vec4& color)
			{
				s_RendererAPI->ClearColor(color);
			}

			static void ClearDepth()
			{
				s_RendererAPI->ClearDepth();
			}

			static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
			{
				s_RendererAPI->DrawIndexed(vertexArray, indexCount);
			}

			static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
			{
				s_RendererAPI->DrawLines(vertexArray, vertexCount);
			}

			static void SetLineWidth(float width)
			{
				s_RendererAPI->SetLineWidth(width);
			}


		private:
			static RendererAPI* s_RendererAPI;
	};
}
