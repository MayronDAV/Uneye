#pragma once


#include "Uneyepch.h"

#include "Core.h"
#include "Uneye/Window.h"
#include <Uneye/Events/ApplicationEvent.h>


namespace Uneye {

	class UNEYE_API Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();

			void OnEvent(Event& e);

		private:
			bool OnWindowClose(WindowCloseEvent& e);

			std::unique_ptr<Window> m_Window;
			bool m_Running = true;
	};


	// Defined by CLIENT
	Application* CreateApplication();
};

