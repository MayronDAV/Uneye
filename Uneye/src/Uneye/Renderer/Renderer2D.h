#pragma once

#include "Uneye/Renderer/Camera.h"
#include "Uneye/Renderer/Texture.h"
#include "Uneye/Renderer/SubTexture.h"

#include "Uneye/Renderer/EditorCamera.h"
#include "Uneye/Renderer/Font.h"

#include "Uneye/Scene/Components.h"



namespace Uneye
{
	class Renderer2D
	{
		public:

			static void Init();
			static void Shutdown();

			static void BeginScene(const Camera& p_camera, const glm::mat4& p_transform);
			static void BeginScene(const EditorCamera& p_camera);
			static void EndScene();
			static void Flush();


			static void DrawQuad(const glm::vec2& p_position, const glm::vec2& p_size, const glm::vec4& p_color = glm::vec4(1.0f), const Ref<Texture2D>& p_texture = nullptr, int p_entityID = -1, uint64_t p_sceneHandle = 0);
			static void DrawQuad(const glm::vec3& p_position, const glm::vec2& p_size, const glm::vec4& p_color = glm::vec4(1.0f), const Ref<Texture2D>& p_texture = nullptr, int p_entityID = -1, uint64_t p_sceneHandle = 0);
			static void DrawQuad(const glm::mat4& p_transform, const glm::vec4& p_color = glm::vec4(1.0f), const Ref<Texture2D>& p_texture = nullptr, int p_entityID = -1, uint64_t p_sceneHandle = 0);

			static void DrawQuad(const glm::vec2& p_position, const glm::vec2& p_size, const Ref<SubTexture2D>& p_subtexture = nullptr, const glm::vec4& p_color = glm::vec4(1.0f), int p_entityID = -1, uint64_t p_sceneHandle = 0);
			static void DrawQuad(const glm::vec3& p_position, const glm::vec2& p_size, const Ref<SubTexture2D>& p_subtexture = nullptr, const glm::vec4& p_color = glm::vec4(1.0f), int p_entityID = -1, uint64_t p_sceneHandle = 0);
			static void DrawQuad(const glm::mat4& p_transform, const Ref<SubTexture2D>& p_subtexture = nullptr, const glm::vec4& p_color = glm::vec4(1.0f), int p_entityID = -1, uint64_t p_sceneHandle = 0);

			static void DrawSprite(const glm::mat4& p_transform, SpriteComponent& p_sc, int p_entityID, uint64_t p_sceneHandle);

			static void DrawCircle(const glm::mat4& p_transform, const glm::vec4& p_color, float p_thickness = 1.0f, float p_fade = 0.005f, int p_entityID = -1, uint64_t p_sceneHandle = 0);

			static void DrawLine(const glm::vec3& p_p0, const glm::vec3& p_p1, const glm::vec4& p_color, int p_entityID = -1, uint64_t p_sceneHandle = 0);

			static void DrawRect(const glm::vec3& p_position, const glm::vec2& p_size, const glm::vec4& p_color = glm::vec4(1), int p_entityID = -1, uint64_t p_sceneHandle = 0);
			static void DrawRect(const glm::mat4& p_transform, const glm::vec4& p_color = glm::vec4(1), int p_entityID = -1, uint64_t p_sceneHandle = 0);

			struct TextParams
			{
				glm::vec4 Color{ 1.0f };
				float Kerning = 0.0f;
				float LineSpacing = 0.0f;
			};
			static void DrawString(const std::string& p_string, Ref<Font> p_font, const glm::mat4& p_transform, const TextParams& p_textParams, int p_entityID = -1, uint64_t p_sceneHandle = 0);
			static void DrawString(const std::string& p_string, const glm::mat4& p_transform, const TextComponent& p_component, int p_entityID = -1, uint64_t p_sceneHandle = 0);


			static float GetLineWidth();
			static void  SetLineWidth(float p_width);

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
