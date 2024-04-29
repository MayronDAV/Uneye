#pragma once

#include "Uneye/Core/Core.h"

#include "Uneye/Core/Window.h"
#include "Uneye/Core/LayerStack.h"
#include "Uneye/Events/Event.h"
#include "Uneye/Events/ApplicationEvent.h"

#include "Uneye/ImGui/ImGuiLayer.h"

#include "Uneye/Renderer/Shader.h"
#include "Uneye/Renderer/Buffer.h"
#include "Uneye/Renderer/VertexArray.h"

#include "Uneye/Core/Timestep.h"


namespace Uneye {

	class Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);

			inline Window& GetWindow() { return *m_Window; }

			inline static Application& Get() { return *s_Instance; }
		private:
			bool OnWindowClose(WindowCloseEvent& e);

		private:
			Scope<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			LayerStack m_LayerStack;
			float m_LastFrameTime = 0.0f;

		private:
			static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}