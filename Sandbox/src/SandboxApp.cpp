#include <Uneye.h>

#include "imgui/imgui.h"

class ExampleLayer : public Uneye::Layer
{
	public:
		ExampleLayer()
			: Layer("Example")
		{
		}

		void OnAttach() override
		{
			std::string vertexSrc = R"(
				#version 330 core
							
				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;

				out vec3 v_pos;
				out vec4 v_color;
			
				void main()
				{
					v_pos = a_Position;
					v_color = a_Color;
					gl_Position = vec4(a_Position, 1.0f);
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
			
				void main()
				{
					v_pos = a_Position;
					gl_Position = vec4(a_Position, 1.0f);
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

		void OnUpdate() override
		{
			Uneye::Renderer::BeginScene();
			{
				m_SquareShader->Bind();
				Uneye::Renderer::Submit(m_SquareVA);

				m_Shader->Bind();
				Uneye::Renderer::Submit(m_VertexArray);
			}
			Uneye::Renderer::EndScene();

			if (Uneye::Input::IsKeyPressed(Uneye::Key::Tab))
				UNEYE_TRACE("Tab key is pressed (poll)!");
		}

		virtual void OnImGuiRender() override
		{
			ImGui::Begin("Test");
			ImGui::Text("Hello World");
			ImGui::End();
		}

		void OnEvent(Uneye::Event& event) override
		{
			if (event.GetEventType() == Uneye::EventType::KeyPressed)
			{
				Uneye::KeyPressedEvent& e = (Uneye::KeyPressedEvent&)event;
				if (e.GetKeyCode() == Uneye::Key::Tab)
					UNEYE_TRACE("Tab key is pressed (event)!");
				UNEYE_TRACE("{0}", (char)e.GetKeyCode());
			}
		}

	private:

		std::shared_ptr<Uneye::Shader> m_Shader;
		std::shared_ptr<Uneye::VertexArray> m_VertexArray;

		std::shared_ptr<Uneye::Shader> m_SquareShader;
		std::shared_ptr<Uneye::VertexArray> m_SquareVA;

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