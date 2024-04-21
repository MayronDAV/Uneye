#pragma once

#include "Core.h"
#include "Uneye/Window.h"
#include <Uneye/Events/Event.h>
#include <Uneye/Events/ApplicationEvent.h>
#include "Uneye/LayerStack.h"


namespace Uneye {

	class UNEYE_API Application
	{
		public:
			Application();
			virtual ~Application() = default;

			void Run();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* overlay);

		private:
			bool OnWindowClose(WindowCloseEvent& e);

			std::unique_ptr<Window> m_Window;
			bool m_Running = true;

			LayerStack m_LayerStack;
	};


	// Defined by CLIENT
	Application* CreateApplication();
};

