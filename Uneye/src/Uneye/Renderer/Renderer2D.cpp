#include "uypch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"



namespace Uneye
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		// TODO: texhandleID
	};


	struct Renderer2Ddata
	{
		static const uint32_t MaxQuads = 5000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: Bindless texture or Rendercaps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};
	static Renderer2Ddata s_Data;


	void Renderer2D::Init()
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" }
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


		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		s_Data.QuadShader = Shader::Create("assets/shaders/square.glsl");
		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		memset(s_Data.TextureSlots.data(), 0, s_Data.TextureSlots.size() * sizeof(uint32_t));
	
		// Set all textures slots to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
		
	}

	void Renderer2D::Shutdown()
	{
		UNEYE_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		UNEYE_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		UNEYE_PROFILE_FUNCTION();

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, 
		const glm::vec4& color, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
		const glm::vec4& color, const Ref<Texture2D>& texture)
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
		if (texture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
				++s_Data.TextureSlotIndex;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = QuadTexCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotateQuad(const glm::vec2& position,
		const glm::vec2& size, float rotation,
		const glm::vec4& color, const Ref<Texture2D>& texture)
	{
		DrawRotateQuad({ position.x, position.y, 0.0f }, size, 
			rotation, color, texture);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec3& position,
		const glm::vec2& size, float rotation,
		const glm::vec4& color, const Ref<Texture2D>& texture)
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
		if (texture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
				++s_Data.TextureSlotIndex;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = QuadTexCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
		const Ref<SubTexture2D>& subtexture, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(position, 0.0f), size, subtexture, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, 
		const Ref<SubTexture2D>& subtexture, const glm::vec4& color)
	{
		UNEYE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* texcoord;
		if (subtexture != nullptr)
			texcoord = subtexture->GetTexCoords();
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
		if (subtexture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *subtexture->GetTexture().get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = subtexture->GetTexture();
				++s_Data.TextureSlotIndex;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = texcoord[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotateQuad(const glm::vec2& position, const glm::vec2& size, 
		float rotation, const Ref<SubTexture2D>& subtexture, const glm::vec4& color)
	{
		DrawRotateQuad(glm::vec3(position, 0.0f), size, rotation, subtexture, color);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec3& position, const glm::vec2& size,
		float rotation, const Ref<SubTexture2D>& subtexture, const glm::vec4& color)
	{
		UNEYE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* texcoord;
		if (subtexture != nullptr)
			texcoord = subtexture->GetTexCoords();
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
		if (subtexture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *subtexture->GetTexture().get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = subtexture->GetTexture();
				++s_Data.TextureSlotIndex;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		for (int i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = texcoord[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;



		s_Data.Stats.QuadCount++;
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