#pragma once

#include "Uneye/Renderer/RenderCommand.h"
#include "Uneye/Renderer/Camera.h"
#include "Uneye/Renderer/Shader.h"

#include "Uneye/Renderer/EditorCamera.h"



namespace Uneye
{
	class Renderer
	{
		public:
			static void Init();
			static void OnWindowResize(uint32_t width, uint32_t height);

			static void BeginScene(EditorCamera& camera);
			static void BeginScene(const Camera& camera, const glm::mat4& transform);
			static void EndScene();

			static void Submit(const Ref<Shader>& shader, 
				const Ref<VertexArray>& vertexArray,
				const glm::mat4& transform = glm::mat4(1.0f));

			inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		private:
			struct SceneData
			{
				glm::mat4 ViewProjectionMatrix;
			};

			static SceneData* m_SceneData;

	};

}

