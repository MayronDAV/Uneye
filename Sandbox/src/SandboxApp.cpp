#include <Uneye.h>

#include "imgui/imgui.h"

#include <GLFW/include/GLFW/glfw3.h>
#include <Platform/OpenGL/OpenGLShader.h>



class ExampleLayer : public Uneye::Layer
{
	public:
		ExampleLayer()
			: Layer("Example")
		{
		}

		void OnAttach() override
		{
			m_Camera = Uneye::OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f);

			m_Shader.reset(Uneye::Shader::Create(
				"../Sandbox/assets/shaders/triangle.vert",
				"../Sandbox/assets/shaders/triangle.frag"));

			m_VertexArray.reset(Uneye::VertexArray::Create());

			float vertices[3 * 3] = {
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f
			};
			Uneye::Ref<Uneye::VertexBuffer> vertexBuffer;
			vertexBuffer.reset(Uneye::VertexBuffer::Create(vertices, sizeof(vertices)));
			Uneye::BufferLayout layout = {
				{ Uneye::ShaderDataType::Float3, "a_Position" }
			};

			vertexBuffer->SetLayout(layout);
			m_VertexArray->AddVertexBuffer(vertexBuffer);

			uint32_t indices[3] = {
				0, 1, 2
			};

			Uneye::Ref<Uneye::IndexBuffer> indexBuffer;
			indexBuffer.reset(Uneye::IndexBuffer::Create(indices,
				sizeof(indices) / sizeof(uint32_t)));
			m_VertexArray->SetIndexBuffer(indexBuffer);


			m_SquareShader.reset(Uneye::Shader::Create(
				"../Sandbox/assets/shaders/square.vert", 
				"../Sandbox/assets/shaders/square.frag"));


			m_SquareVA.reset(Uneye::VertexArray::Create());

			float squareVertices[3 * 4] = {
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.5f,  0.5f, 0.0f,
				-0.5f,  0.5f, 0.0f
			};

			Uneye::Ref<Uneye::VertexBuffer> squareVB;
			squareVB.reset(Uneye::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
			squareVB->SetLayout({
				{ Uneye::ShaderDataType::Float3, "a_Position" },
				});
			m_SquareVA->AddVertexBuffer(squareVB);

			uint32_t squareIndices[6] = {
				0, 1, 2,
				0, 2, 3,
			};
			Uneye::Ref<Uneye::IndexBuffer> squareIB;
			squareIB.reset(Uneye::IndexBuffer::Create(squareIndices, 
				sizeof(squareIndices) / sizeof(uint32_t)));
			m_SquareVA->SetIndexBuffer(squareIB);

			//Uneye::Application::Get().GetWindow().SetVSync(false);
		}

		void OnUpdate(Uneye::Timestep ts) override
		{
			//UNEYE_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());
			ProcessInput(ts);

			Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });

			//m_Camera.SetRotation(45.0f);
			m_Camera.SetPosition(m_CameraPosition);

			Uneye::Renderer::BeginScene(m_Camera);
			{
				static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			

				std::dynamic_pointer_cast<Uneye::OpenGLShader>(m_SquareShader)->Bind();

				for (float x = 0.0f; x <= 20.0f; x += 1.0f)
				{
					for (float y = 0.0f; y <= 20.0f; y += 1.0f)
					{
						glm::mat4 model(1.0f);
						model = glm::translate(model, glm::vec3(x * 0.11f, y * 0.11f, 0.0f)) * scale;
						if ((int)x % 2 == 0)
							std::dynamic_pointer_cast<Uneye::OpenGLShader>(m_SquareShader)->SetVec4("u_Color", m_SquareColor1);
						else
							std::dynamic_pointer_cast<Uneye::OpenGLShader>(m_SquareShader)->SetVec4("u_Color", m_SquareColor2);

						Uneye::Renderer::Submit(m_SquareShader, m_SquareVA, model);
						
					}
				}

				glm::mat4 model(1.0f);
				model = glm::translate(model, m_CameraPosition) * scale;
				std::dynamic_pointer_cast<Uneye::OpenGLShader>(m_Shader)->Bind();
				std::dynamic_pointer_cast<Uneye::OpenGLShader>(m_Shader)->SetVec4("u_Color", m_TriangleColor);
				Uneye::Renderer::Submit(m_Shader, m_VertexArray, model);
			}
			Uneye::Renderer::EndScene();
		}

		void ProcessInput(Uneye::Timestep ts)
		{
			m_Direction = glm::vec3(0.0f);

			if (Uneye::Input::IsKeyPressed(Uneye::Key::W))
				m_Direction.y = 1.0f;
			if (Uneye::Input::IsKeyPressed(Uneye::Key::A))
				m_Direction.x = -1.0f;
			if (Uneye::Input::IsKeyPressed(Uneye::Key::S))
				m_Direction.y = -1.0f;
			if (Uneye::Input::IsKeyPressed(Uneye::Key::D))
				m_Direction.x = 1.0f;

			m_CameraPosition += (m_Direction / ((glm::length(m_Direction) != 0) ?
				glm::length(m_Direction) : 1.0f)) * m_Speed * ts.GetSeconds();
		}

		virtual void OnImGuiRender() override
		{
			ImGui::Begin("Quads Setup");
			ImGui::ColorEdit4("SquareColor1", m_SquareColor1);
			ImGui::ColorEdit4("SquareColor2", m_SquareColor2);
			ImGui::ColorEdit4("TriangleColor", m_TriangleColor);
			ImGui::End();

		}

		void OnEvent(Uneye::Event& event) override
		{
		}

	private:

		Uneye::Ref<Uneye::Shader> m_Shader;
		Uneye::Ref<Uneye::VertexArray> m_VertexArray;

		Uneye::Ref<Uneye::Shader> m_SquareShader;
		Uneye::Ref<Uneye::VertexArray> m_SquareVA;

		Uneye::OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition{ 0.0f };
		glm::vec3 m_Direction{ 0.0f };
		glm::vec3 m_Speed{ 0.5f };

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