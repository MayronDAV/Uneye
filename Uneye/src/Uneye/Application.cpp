#include "Uneyepch.h"

#include "Application.h"

#include <Uneye/Log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Uneye/Input.h"


namespace Uneye {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		UNEYE_CORE_ASSERT(s_Instance, "s_Instance already exists");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(1.0f, 0.5f, 0.0f, 1.0f);

			
			{
				for (auto layer : m_LayerStack)
					layer->OnUpdate();
			}

			m_ImGuiLayer->Begin();
			{
				for (auto layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		UNEYE_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
};