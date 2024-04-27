#include "uypch.h"
#include "Uneye/Application.h"

#include "Uneye/Log.h"

#include <glad/glad.h>

#include "Uneye/Input.h"


namespace Uneye {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Uneye::ShaderDataType::Float:		return GL_FLOAT;
			case Uneye::ShaderDataType::Float2:		return GL_FLOAT;
			case Uneye::ShaderDataType::Float3:		return GL_FLOAT;
			case Uneye::ShaderDataType::Float4:		return GL_FLOAT;
			case Uneye::ShaderDataType::Mat3:		return GL_FLOAT;
			case Uneye::ShaderDataType::Mat4:		return GL_FLOAT;
			case Uneye::ShaderDataType::Int:		return GL_INT;
			case Uneye::ShaderDataType::Int2:		return GL_INT;
			case Uneye::ShaderDataType::Int3:		return GL_INT;
			case Uneye::ShaderDataType::Int4:		return GL_INT;
			case Uneye::ShaderDataType::Bool:		return GL_BOOL;
		}

		UNEYE_CORE_ASSERT(true, "Unknown ShaderDataType!");
		return 0;
	}

	Application::Application() 
	{
		UNEYE_CORE_ASSERT(s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

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


		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		glCreateVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		{

			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" }
			};

			m_VertexBuffer->SetLayout(layout);
		}

		uint32_t index = 0;
		const auto& layout = m_VertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				(element.Normalized) ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}

		uint32_t indices[3] = {
			0, 1, 2
		};
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));


		glBindVertexArray(0);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
			{
				UNEYE_CORE_TRACE("{0} -> {1}",(*it)->GetName(), e);
				break;
			}
		}
	}

	void Application::Run()
	{
		auto color = m_ImGuiLayer->GetColorBG();

		while (m_Running)
		{
			glClearColor(color[0], color[1], color[2], color[3]);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();


			glBindVertexArray(m_VAO);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}