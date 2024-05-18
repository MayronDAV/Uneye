#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Renderer/Texture.h"
#include "Uneye/Renderer/SubTexture.h"

#include "Uneye/Renderer/EditorCamera.h"

#include "Uneye/Scene/Components.h"



namespace Uneye
{
	class Renderer2D
	{
		public:

			static void Init();
			static void Shutdown();

			static void BeginScene(const Camera& camera, const glm::mat4& transform);
			static void BeginScene(const EditorCamera& camera);
			static void EndScene();
			static void Flush();


			static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
				const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);
			static void DrawQuad(const glm::vec3& position, const glm::vec2& size,
				const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);		
			static void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f),
				const Ref<Texture2D>& texture = nullptr, int entityID = -1);


			static void DrawRotateQuad(const glm::vec2& position, const glm::vec2& size,
				float rotation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);
			static void DrawRotateQuad(const glm::vec3& position, const glm::vec2& size,
				float rotation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr);


			static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
				const Ref<SubTexture2D>& subtexture = nullptr, const glm::vec4& color = glm::vec4(1.0f));
			static void DrawQuad(const glm::vec3& position, const glm::vec2& size,
				const Ref<SubTexture2D>& subtexture = nullptr, const glm::vec4& color = glm::vec4(1.0f));
			static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture = nullptr,
				const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);


			static void DrawRotateQuad(const glm::vec2& position, const glm::vec2& size,
				float rotation, const Ref<SubTexture2D>& subtexture = nullptr, const glm::vec4& color = glm::vec4(1.0f));
			static void DrawRotateQuad(const glm::vec3& position, const glm::vec2& size,
				float rotation, const Ref<SubTexture2D>& subtexture = nullptr, const glm::vec4& color = glm::vec4(1.0f));

			static void DrawSprite(const glm::mat4& transform, SpriteComponent& mc, int entityID);

			static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

			static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);

			static void DrawRect(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color = glm::vec4(1), int entityID = -1);
			static void DrawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1), int entityID = -1);

			static float GetLineWidth();
			static void  SetLineWidth(float width);

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
			static void StartBatch();
			static void FlushAndReset();
	
	};



}
