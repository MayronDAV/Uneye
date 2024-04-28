#pragma once

#include "Uneye/Renderer/RendererAPI.h"


namespace Uneye
{
	class OpenGLRendererAPI : public RendererAPI
	{
		public:
			virtual void Clear(const glm::vec4& color) const override;
			virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};

}