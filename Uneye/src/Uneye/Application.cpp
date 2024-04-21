#include "Uneyepch.h"

#include "Application.h"

#include <Uneye/Log.h>

#include <GLFW/glfw3.h>


namespace Uneye {

#define BIND_EVENT_FN(x) {std::bind(&Application::x, this, std::placeholders::_1)}

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(1.0f, 0.5f, 0.0f, 1.0f);

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		UNEYE_CORE_TRACE("{0}", e);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
};