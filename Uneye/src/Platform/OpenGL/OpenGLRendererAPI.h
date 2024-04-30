#pragma once

#include "Uneye/Renderer/RendererAPI.h"


namespace Uneye
{
	class OpenGLRendererAPI : public RendererAPI
	{
		public:
			virtual void Init() override;

			virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

			virtual void Clear(const glm::vec4& color) const override;
			virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};

}