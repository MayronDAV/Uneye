#pragma once

#include "Uneye/Core/Base.h"

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

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			UNEYE_CORE_ASSERT(index < Count, "");
			return Args[index];
		}
	};

	class Application
	{
		public:
			Application(const std::string& name = "Uneye App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
			virtual ~Application();

			void Run();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);
			
			void Close() { m_Running = false; }

			inline Window& GetWindow() { return *m_Window; }
			inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer;  }

			inline static Application& Get() { return *s_Instance; }

			ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
		private:
			bool OnWindowClose(WindowCloseEvent& e);
			bool OnWindowResize(WindowResizeEvent& e);

		private:
			ApplicationCommandLineArgs m_CommandLineArgs;
			Scope<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			bool m_Minimized = false;
			LayerStack m_LayerStack;
			float m_LastFrameTime = 0.0f;

		private:
			static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
	Application* CreateApplication(ApplicationCommandLineArgs args);
}
