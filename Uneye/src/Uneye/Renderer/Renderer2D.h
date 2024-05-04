#pragma once

#include "Camera.h"
#include "Texture.h"

namespace Uneye
{
	class Renderer2D
	{
		public:
			static void Init();
			static void Shutdown();

			static void BeginScene(const OrthographicCamera& camera);
			static void EndScene();
			static void Flush();

			// Primitives
			static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
				const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);
			static void DrawQuad(const glm::vec3& position, const glm::vec2& size,
				const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);

			static void DrawRotateQuad(const glm::vec2& position, const glm::vec2& size,
				float rotation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);
			static void DrawRotateQuad(const glm::vec3& position, const glm::vec2& size,
				float rotation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);
	
			// Stats
			struct Statistics
			{
				uint32_t DrawCalls = 0;
				uint32_t QuadCount = 0;

				uint32_t GetTotalVertexCount() { return QuadCount * 4; }
				uint32_t GetTotalIndexCount() { return QuadCount * 6; }
			};
			static Statistics GetStats();
			static void ResetStats();

		private:

			static void FlushAndReset();
	
	};



}