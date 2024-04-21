#include "Uneyepch.h"

#include "Application.h"

#include <Uneye/Events/ApplicationEvent.h>
#include <Uneye/Log.h>

#include <GLFW/glfw3.h>


namespace Uneye {

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
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
};