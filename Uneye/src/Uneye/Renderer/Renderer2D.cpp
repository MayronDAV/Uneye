#include "uypch.h"
#include "Uneye/Renderer/Renderer2D.h"
#include "Uneye/Renderer/VertexArray.h"
#include "Uneye/Renderer/Shader.h"
#include "Uneye/Renderer/RenderCommand.h"

#include "Uneye/Renderer/UniformBuffer.h"

#include "Uneye/Asset/AssetManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MSDFData.h"


namespace Uneye
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;

		// Editor only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor only
		int EntityID;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;

		// TODO: bg color for outline/bg

		// Editor-only
		int EntityID;
	};

	struct Renderer2Ddata
	{
		// ----------------------------
		// QUAD stuff
		static const uint32_t MaxQuads = 5000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		glm::vec4 QuadVertexPositions[4];

		// ----------------------------
		// CIRCLE Stuff

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;


		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		// ----------------------------
		// LINE Stuff

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;


		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		// ----------------------------
		// TEXT Stuff

		Ref<VertexArray> TextVertexArray;
		Ref<VertexBuffer> TextVertexBuffer;
		Ref<Shader> TextShader;

		uint32_t TextIndexCount = 0;
		TextVertex* TextVertexBufferBase = nullptr;
		TextVertex* TextVertexBufferPtr = nullptr;

		Ref<Texture2D> FontAtlasTexture;

		// ----------------------------
		// OTHER stuff

		Ref<Texture2D> WhiteTexture;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;


		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer{ glm::mat4(1.0f) };
		Ref<UniformBuffer> CameraUniformBuffer = nullptr;
	};

	static Renderer2Ddata s_Data;


	void Renderer2D::Init()
	{
		UNEYE_PROFILE_FUNCTION();

		// ----------------------------
		// QUAD stuff
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"	   },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float , "a_TexIndex" },
			{ ShaderDataType::Int,	  "a_EntityID" },
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 0;
			quadIndices[i + 4] = offset + 2;
			quadIndices[i + 5] = offset + 3;

			offset += 4;
		}

		 Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

		delete[] quadIndices;

		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		// ----------------------------
		// CIRCLE stuff

		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition"  },
			{ ShaderDataType::Float3, "a_LocalPosition"  },
			{ ShaderDataType::Float4, "a_Color"			 },
			{ ShaderDataType::Float	, "a_Thickness"		 },
			{ ShaderDataType::Float	, "a_Fade"			 },
			{ ShaderDataType::Int,	  "a_EntityID" },
		});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB); // Use quadIB
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");

		// ----------------------------
		// LINE stuff

		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"  },
			{ ShaderDataType::Float4, "a_Color"	    },
			{ ShaderDataType::Int,	  "a_EntityID" },
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");

		// ----------------------------
		// TEXT Stuff

		s_Data.TextVertexArray = VertexArray::Create();

		s_Data.TextVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(TextVertex));
		s_Data.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Int,	  "a_EntityID" },
			});
		s_Data.TextVertexArray->AddVertexBuffer(s_Data.TextVertexBuffer);
		s_Data.TextVertexArray->SetIndexBuffer(quadIB);
		s_Data.TextVertexBufferBase = new TextVertex[s_Data.MaxVertices];

		s_Data.TextShader = Shader::Create("assets/shaders/Renderer2D_Text.glsl");

		// ----------------------------
		// OTHER stuff

		s_Data.WhiteTexture = Texture2D::Create(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(Buffer(&whiteTextureData, sizeof(uint32_t)));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		memset(s_Data.TextureSlots.data(), 0, s_Data.TextureSlots.size() * sizeof(uint32_t));
	
		// Set all textures slots to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
		
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2Ddata::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		UNEYE_PROFILE_FUNCTION();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextIndexCount = 0;
		s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const Camera& p_camera, const glm::mat4& p_transform)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = p_camera.GetProjection() * glm::inverse(p_transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2Ddata::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& p_camera)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = p_camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2Ddata::CameraData));

		StartBatch();
	}


	void Renderer2D::EndScene()
	{
		UNEYE_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::Flush()
	{
		UNEYE_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);

			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.TextIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.TextVertexBufferPtr - (uint8_t*)s_Data.TextVertexBufferBase);
			s_Data.TextVertexBuffer->SetData(s_Data.TextVertexBufferBase, dataSize);

			auto buf = s_Data.TextVertexBufferBase;
			s_Data.FontAtlasTexture->Bind(0);

			s_Data.TextShader->Bind();
			RenderCommand::DrawIndexed(s_Data.TextVertexArray, s_Data.TextIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		StartBatch();
	}




	void Renderer2D::DrawQuad(const glm::vec2& p_position, const glm::vec2& p_size, 
		const glm::vec4& p_color, const Ref<Texture2D>& p_texture, int p_entityID)
	{
		DrawQuad({ p_position.x, p_position.y, 0.0f }, p_size, p_color, p_texture, p_entityID);
	}

	void Renderer2D::DrawQuad(const glm::vec3& p_position, const glm::vec2& p_size,
		const glm::vec4& p_color, const Ref<Texture2D>& p_texture, int p_entityID)
	{
		UNEYE_PROFILE_FUNCTION();
		UNEYE_CORE_VERIFY(!p_texture);

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 QuadTexCoords[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		if (s_Data.QuadIndexCount >= Renderer2Ddata::MaxVertices)
			FlushAndReset();

		float textureIndex = 0.0f; // White texture
		if (p_texture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *p_texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (s_Data.TextureSlotIndex >= Renderer2Ddata::MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = p_texture;
				s_Data.TextureSlotIndex++;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), p_position);
		transform = glm::scale(transform, { p_size.x, p_size.y, 1.0f });

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = p_color;
			s_Data.QuadVertexBufferPtr->TexCoord = QuadTexCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->EntityID = p_entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& p_transform, const glm::vec4& p_color,
		const Ref<Texture2D>& p_texture, int p_entityID)
	{
		UNEYE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 QuadTexCoords[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		if (s_Data.QuadIndexCount >= Renderer2Ddata::MaxVertices)
			FlushAndReset();

		float textureIndex = 0.0f; // White texture
		if (p_texture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *p_texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (s_Data.TextureSlotIndex >= Renderer2Ddata::MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = p_texture;
				s_Data.TextureSlotIndex++;
			}
		}

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = p_transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = p_color;
			s_Data.QuadVertexBufferPtr->TexCoord = QuadTexCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->EntityID = p_entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
	}




	void Renderer2D::DrawQuad(const glm::vec2& p_position, const glm::vec2& p_size,
		const Ref<SubTexture2D>& p_subtexture, const glm::vec4& p_color, int p_entityID)
	{
		DrawQuad(glm::vec3(p_position, 0.0f), p_size, p_subtexture, p_color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& p_position, const glm::vec2& p_size,
		const Ref<SubTexture2D>& p_subtexture, const glm::vec4& p_color, int p_entityID)
	{
		UNEYE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* texcoord;
		if (p_subtexture != nullptr)
			texcoord = p_subtexture->GetTexCoords();
		else
		{
			texcoord = new glm::vec2[4]{
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f},
			};
		}

		if (s_Data.QuadIndexCount >= Renderer2Ddata::MaxVertices)
			FlushAndReset();

		float textureIndex = 0.0f; // White texture
		if (p_subtexture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *(p_subtexture->GetTexture().get()))
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (s_Data.TextureSlotIndex >= Renderer2Ddata::MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = p_subtexture->GetTexture();
				++s_Data.TextureSlotIndex;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), p_position);
		transform = glm::scale(transform, { p_size.x, p_size.y, 1.0f });

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = p_color;
			s_Data.QuadVertexBufferPtr->TexCoord = texcoord[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->EntityID = p_entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& p_transform, const Ref<SubTexture2D>& p_subtexture,
		const glm::vec4& p_color, int p_entityID)
	{
		UNEYE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* texcoord;
		if (p_subtexture != nullptr)
			texcoord = p_subtexture->GetTexCoords();
		else
		{
			texcoord = new glm::vec2[4]{
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f},
			};
		}

		if (s_Data.QuadIndexCount >= Renderer2Ddata::MaxVertices)
			FlushAndReset();

		float textureIndex = 0.0f; // White texture
		if (p_subtexture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*(s_Data.TextureSlots[i].get()) == *(p_subtexture->GetTexture().get()))
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (s_Data.TextureSlotIndex >= Renderer2Ddata::MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = p_subtexture->GetTexture();
				++s_Data.TextureSlotIndex;
			}
		}

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = p_transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = p_color;
			s_Data.QuadVertexBufferPtr->TexCoord = texcoord[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->EntityID = p_entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
	}



	void Renderer2D::DrawSprite(const glm::mat4& p_transform, SpriteComponent& p_sc, int p_entityID)
	{
		Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(p_sc.Texture);

		if (p_sc.IsSubTexture)
		{
			try
			{
				Ref<SubTexture2D> subtexture = SubTexture2D::CreateFromTexture(texture, p_sc.TileSize, p_sc.TileCoord, p_sc.SpriteSize);
				DrawQuad(p_transform, subtexture, p_sc.Color, p_entityID);
			}
			catch (...)
			{

			}
		}
		else
		{
			try
			{
				DrawQuad(p_transform, p_sc.Color, texture, p_entityID);
			}
			catch (...)
			{

			}
		}
	}

	void Renderer2D::DrawCircle(const glm::mat4& p_transform, const glm::vec4& p_color, float p_thickness, float p_fade, int p_entityID)
	{
		UNEYE_PROFILE_FUNCTION();

		// Implement for circles
		//if (s_Data.QuadIndexCount >= Renderer2Ddata::MaxVertices)
		//	FlushAndReset();


		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = p_transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = p_color;
			s_Data.CircleVertexBufferPtr->Thickness = p_thickness;
			s_Data.CircleVertexBufferPtr->Fade = p_fade;
			s_Data.CircleVertexBufferPtr->EntityID = p_entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p_p0, const glm::vec3& p_p1, const glm::vec4& p_color, int p_entityID)
	{
		s_Data.LineVertexBufferPtr->Position = p_p0;
		s_Data.LineVertexBufferPtr->Color = p_color;
		s_Data.LineVertexBufferPtr->EntityID = p_entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p_p1;
		s_Data.LineVertexBufferPtr->Color = p_color;
		s_Data.LineVertexBufferPtr->EntityID = p_entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& p_position, const glm::vec2& p_size, const glm::vec4& p_color, int p_entityID)
	{
		glm::vec3 p0 = glm::vec3(p_position.x - p_size.x * 0.5f, p_position.y - p_size.y * 0.5f, p_position.z);
		glm::vec3 p1 = glm::vec3(p_position.x + p_size.x * 0.5f, p_position.y - p_size.y * 0.5f, p_position.z);
		glm::vec3 p2 = glm::vec3(p_position.x + p_size.x * 0.5f, p_position.y + p_size.y * 0.5f, p_position.z);
		glm::vec3 p3 = glm::vec3(p_position.x - p_size.x * 0.5f, p_position.y + p_size.y * 0.5f, p_position.z);

		DrawLine(p0, p1, p_color, p_entityID);
		DrawLine(p1, p2, p_color, p_entityID);
		DrawLine(p2, p3, p_color, p_entityID);
		DrawLine(p3, p0, p_color, p_entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& p_transform, const glm::vec4& p_color, int p_entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = p_transform * s_Data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], p_color, p_entityID);
		DrawLine(lineVertices[1], lineVertices[2], p_color, p_entityID);
		DrawLine(lineVertices[2], lineVertices[3], p_color, p_entityID);
		DrawLine(lineVertices[3], lineVertices[0], p_color, p_entityID);
	}

	void Renderer2D::DrawString(const std::string& p_string, Ref<Font> p_font, const glm::mat4& p_transform,
		const TextParams& p_textParams, int p_entityID)
	{
		const auto& fontGeometry = p_font->GetMSDFData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		Ref<Texture2D> fontAtlas = p_font->GetAtlasTexture();

		s_Data.FontAtlasTexture = fontAtlas;

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;
		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		for (size_t i = 0; i < p_string.size(); i++)
		{
			char character = p_string[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight + p_textParams.LineSpacing;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphAdvance;
				if (i < p_string.size() - 1)
				{
					char nextCharacter = p_string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance + p_textParams.Kerning;
				continue;
			}

			if (character == '\t')
			{
				// NOTE: is this right?
				x += 4.0f * (fsScale * spaceGlyphAdvance + p_textParams.Kerning);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			if (character == '\t')
				glyph = fontGeometry.getGlyph(' ');

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->GetWidth();
			float texelHeight = 1.0f / fontAtlas->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			s_Data.TextVertexBufferPtr->Position = p_transform * glm::vec4(quadMin, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = p_textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = texCoordMin;
			s_Data.TextVertexBufferPtr->EntityID = p_entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = p_transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = p_textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			s_Data.TextVertexBufferPtr->EntityID = p_entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = p_transform * glm::vec4(quadMax, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = p_textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = texCoordMax;
			s_Data.TextVertexBufferPtr->EntityID = p_entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = p_transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = p_textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			s_Data.TextVertexBufferPtr->EntityID = p_entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextIndexCount += 6;
			s_Data.Stats.QuadCount++;

			if (i < p_string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = p_string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				float kerningOffset = 0.0f;
				x += fsScale * advance + p_textParams.Kerning;;
			}
		}
	}

	void Renderer2D::DrawString(const std::string& string, const glm::mat4& p_transform,
		const TextComponent& component, int p_entityID)
	{
		DrawString(string, component.FontAsset, p_transform, { component.Color, component.Kerning, component.LineSpacing }, p_entityID);
	}


	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float p_width)
	{
		s_Data.LineWidth = p_width;
	}


	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

}
