#include <Uneye.h>

#include "imgui/imgui.h"

#include <GLFW/include/GLFW/glfw3.h>
#include <Platform/OpenGL/OpenGLShader.h>

#include "Uneye/Renderer/Shader.h"



class ExampleLayer : public Uneye::Layer
{
	public:
		ExampleLayer()
			: Layer("Example"),
			m_CameraController(1.67)
		{
		}

		void OnAttach() override
		{
			m_SquareVA = Uneye::VertexArray::Create();

			float squareVertices[5 * 4] = {
				-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
			};

			Uneye::Ref<Uneye::VertexBuffer> squareVB;
			squareVB = Uneye::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
			squareVB->SetLayout({
				{ Uneye::ShaderDataType::Float3, "a_Position" },
				{ Uneye::ShaderDataType::Float2, "a_TexCoord" }
			});
			m_SquareVA->AddVertexBuffer(squareVB);

			uint32_t squareIndices[6] = {
				0, 1, 2,
				0, 2, 3,
			};
			Uneye::Ref<Uneye::IndexBuffer> squareIB;
			squareIB = Uneye::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
			m_SquareVA->SetIndexBuffer(squareIB);


			auto textureShader = m_ShaderLibrary.Load("assets/shaders/texture.glsl");

			m_TextureContainer = Uneye::Texture2D::Create("assets/textures/container.jpg");
			m_TextureWall = Uneye::Texture2D::Create("assets/textures/wall.jpg");
			m_TextureGrass = Uneye::Texture2D::Create("assets/textures/blending_transparent_window.png");

			Uneye::Application::Get().GetWindow().SetVSync(false);
		}

		void OnUpdate(Uneye::Timestep ts) override
		{
			m_CameraController.OnUpdate(ts);

			Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });

			auto textureShader = m_ShaderLibrary.Get("texture");
			Uneye::Renderer::BeginScene(m_CameraController.GetCamera());
			{
				static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

				m_TextureContainer->Bind(0);
				for (float x = 0.0f; x <= 20.0f; x += 1.0f)
				{
					for (float y = 0.0f; y <= 20.0f; y += 1.0f)
					{
						glm::mat4 model(1.0f);
						model = glm::translate(model, glm::vec3(x * 0.11f, y * 0.11f, 0.0f)) * scale;

						Uneye::Renderer::Submit(textureShader, m_SquareVA, model);
						
					}
				}

				glm::mat4 model(1.0f);
				model = glm::translate(model, m_CameraController.GetPosition()) * scale;
				m_TextureGrass->Bind(0);
				Uneye::Renderer::Submit(textureShader, m_SquareVA, model);

			}
			Uneye::Renderer::EndScene();
		}

		void OnEvent(Uneye::Event& e) override
		{
			m_CameraController.OnEvent(e);
		}

		virtual void OnImGuiRender() override
		{
			ImGui::Begin("Quads Setup");
			ImGui::ColorEdit4("SquareColor1", m_SquareColor1);
			ImGui::ColorEdit4("SquareColor2", m_SquareColor2);
			ImGui::ColorEdit4("TriangleColor", m_TriangleColor);
			ImGui::End();

		}


	private:
		Uneye::ShaderLibrary m_ShaderLibrary;

		Uneye::Ref<Uneye::VertexArray> m_VertexArray;
		Uneye::Ref<Uneye::VertexArray> m_SquareVA;

		Uneye::Ref<Uneye::Texture2D> m_TextureContainer;
		Uneye::Ref<Uneye::Texture2D> m_TextureWall;
		Uneye::Ref<Uneye::Texture2D> m_TextureGrass;

		Uneye::OrthographicCameraController m_CameraController;

		float m_SquareColor1[4]  = { 0.8f, 0.2f, 0.3f, 1.0f };
		float m_SquareColor2[4]  = { 0.2f, 0.3f, 0.8f, 1.0f };
		float m_TriangleColor[4] = { 0.5f, 0.0f, 0.0f, 1.0f };
};

class Sandbox : public Uneye::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() = default;

};

Uneye::Application* Uneye::CreateApplication()
{
	return new Sandbox();
}