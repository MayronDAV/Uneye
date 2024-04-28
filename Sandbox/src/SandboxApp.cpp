#include <Uneye.h>

#include "imgui/imgui.h"

#include <GLFW/include/GLFW/glfw3.h>



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

			std::string vertexSrc = R"(
				#version 330 core
							
				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;

				out vec3 v_pos;
				out vec4 v_color;

				uniform mat4 u_ViewProjection;
			
				void main()
				{
					v_pos = a_Position;
					v_color = a_Color;
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
				}
			)";

			std::string fragmentSrc = R"(
				#version 330 core
							
				out vec4 color;

				in vec3 v_pos;
				in vec4 v_color;

				void main()
				{
					color = vec4(v_pos * 0.5f + 0.5f, 1.0f);
					color = v_color;
				}
			)";


			m_Shader.reset(new Uneye::Shader(vertexSrc, fragmentSrc));

			m_VertexArray.reset(Uneye::VertexArray::Create());

			float vertices[3 * 7] = {
				-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
				 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
				 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			};
			std::shared_ptr<Uneye::VertexBuffer> vertexBuffer;
			vertexBuffer.reset(Uneye::VertexBuffer::Create(vertices, sizeof(vertices)));
			Uneye::BufferLayout layout = {
				{ Uneye::ShaderDataType::Float3, "a_Position" },
				{ Uneye::ShaderDataType::Float4, "a_Color" }
			};

			vertexBuffer->SetLayout(layout);
			m_VertexArray->AddVertexBuffer(vertexBuffer);

			uint32_t indices[3] = {
				0, 1, 2
			};

			std::shared_ptr<Uneye::IndexBuffer> indexBuffer;
			indexBuffer.reset(Uneye::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
			m_VertexArray->SetIndexBuffer(indexBuffer);



			std::string squareVertexSrc = R"(
				#version 330 core
							
				layout(location = 0) in vec3 a_Position;

				out vec3 v_pos;
			
				uniform mat4 u_ViewProjection;

				void main()
				{
					v_pos = a_Position;
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
				}
			)";

			std::string squareFagmentSrc = R"(
				#version 330 core
							
				out vec4 color;

				in vec3 v_pos;

				void main()
				{
					color = vec4(0.8f, 0.2f, 0.3f, 1.0f);
				}
			)";
			m_SquareShader.reset(new Uneye::Shader(squareVertexSrc, squareFagmentSrc));


			m_SquareVA.reset(Uneye::VertexArray::Create());

			float squareVertices[3 * 4] = {
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.5f,  0.5f, 0.0f,
				-0.5f,  0.5f, 0.0f
			};

			std::shared_ptr<Uneye::VertexBuffer> squareVB;
			squareVB.reset(Uneye::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
			squareVB->SetLayout({
				{ Uneye::ShaderDataType::Float3, "a_Position" },
				});
			m_SquareVA->AddVertexBuffer(squareVB);

			uint32_t squareIndices[6] = {
				0, 1, 2,
				0, 2, 3,
			};
			std::shared_ptr<Uneye::IndexBuffer> squareIB;
			squareIB.reset(Uneye::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
			m_SquareVA->SetIndexBuffer(squareIB);
		}

		void OnUpdate(Uneye::Timestep ts) override
		{
			//UNEYE_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());
			m_Direction = glm::vec3(0.0f);

			if (Uneye::Input::IsKeyPressed(Uneye::Key::W))
				m_Direction.y = -1.0f;
			if (Uneye::Input::IsKeyPressed(Uneye::Key::A))
				m_Direction.x =  1.0f;
			if (Uneye::Input::IsKeyPressed(Uneye::Key::S))
				m_Direction.y =  1.0f;
			if (Uneye::Input::IsKeyPressed(Uneye::Key::D))
				m_Direction.x = -1.0f;

			m_CameraPosition += m_Direction * m_Speed * ts.GetSeconds();

			Uneye::RenderCommand::Clear({ 0.1f, 0.1f, 0.13f, 1.0f });

			//m_Camera.SetRotation(45.0f);
			m_Camera.SetPosition(m_CameraPosition);

			Uneye::Renderer::BeginScene(m_Camera);
			{
				Uneye::Renderer::Submit(m_SquareShader, m_SquareVA);

				Uneye::Renderer::Submit(m_Shader, m_VertexArray);
			}
			Uneye::Renderer::EndScene();
		}

		virtual void OnImGuiRender() override
		{
		}

		void OnEvent(Uneye::Event& event) override
		{
		}

	private:

		std::shared_ptr<Uneye::Shader> m_Shader;
		std::shared_ptr<Uneye::VertexArray> m_VertexArray;

		std::shared_ptr<Uneye::Shader> m_SquareShader;
		std::shared_ptr<Uneye::VertexArray> m_SquareVA;

		Uneye::OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition{ 0.0f };
		glm::vec3 m_Direction{ 0.0f };
		glm::vec3 m_Speed{ 1.0f };

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