#include "uypch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"



namespace Uneye
{

	struct Renderer2Ddata
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;
	};
	static Renderer2Ddata* s_Data;


	void Renderer2D::Init()
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data = new Renderer2Ddata();
		s_Data->QuadVertexArray = VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			});
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = {
			0, 1, 2,
			0, 2, 3,
		};
		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));


		s_Data->QuadShader = Shader::Create("assets/shaders/square.glsl");
	}

	void Renderer2D::Shutdown()
	{
		UNEYE_PROFILE_FUNCTION();

		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data->QuadShader->Bind();
		s_Data->QuadShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
		UNEYE_PROFILE_FUNCTION();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, 
		const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
		const glm::vec4& color)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data->QuadShader->Bind();
		s_Data->QuadShader->SetVec4("u_Color", color);
		s_Data->WhiteTexture->Bind();

		glm::mat4 model(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(size, 1.0f));
		s_Data->QuadShader->SetMat4("u_ModelMatrix", model);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
		const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, 
		const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data->QuadShader->Bind();
		s_Data->QuadShader->SetVec4("u_Color", color);
		texture->Bind(s_Data->QuadShader->GetLocation("u_Texture"));

		glm::mat4 model(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(size, 1.0f));
		s_Data->QuadShader->SetMat4("u_ModelMatrix", model);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotateQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data->QuadShader->Bind();
		s_Data->QuadShader->SetVec4("u_Color", color);
		s_Data->WhiteTexture->Bind();

		glm::mat4 model(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(size, 1.0f));
		s_Data->QuadShader->SetMat4("u_ModelMatrix", model);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		DrawRotateQuad({ position.x, position.y, 0.0f }, size, rotation, texture, color);
	}

	void Renderer2D::DrawRotateQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data->QuadShader->Bind();
		s_Data->QuadShader->SetVec4("u_Color", color);
		texture->Bind(s_Data->QuadShader->GetLocation("u_Texture"));

		glm::mat4 model(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(size, 1.0f));
		s_Data->QuadShader->SetMat4("u_ModelMatrix", model);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}