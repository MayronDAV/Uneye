#include "Uneyepch.h"

#include "Application.h"

#include <Uneye/Events/ApplicationEvent.h>
#include <Uneye/Log.h>


namespace Uneye {

	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		Uneye::WindowResizeEvent e(1280, 600);
		UNEYE_TRACE(e);

		while (true)
		{

		}
	}
};