#pragma once

#include "Core.h"
#include "Uneye/Window.h"
#include <Uneye/Events/Event.h>
#include <Uneye/Events/ApplicationEvent.h>
#include "Uneye/LayerStack.h"
#include "Uneye/ImGui/ImGuiLayer.h"


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

			inline static Application& Get() { return *s_Instance; }
			inline Window& GetWindow() { return *m_Window; }

		private:
			bool OnWindowClose(WindowCloseEvent& e);

			std::unique_ptr<Window> m_Window;
			bool m_Running = true;

			LayerStack m_LayerStack;
			ImGuiLayer* m_ImGuiLayer;

			static Application* s_Instance;
	};


	// Defined by CLIENT
	Application* CreateApplication();
};

