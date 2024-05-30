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
			UNEYE_CORE_ASSERT(index >= Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Uneye Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
		public:
			Application() = default;
			Application(const ApplicationSpecification& specification);
			virtual ~Application();

			void Run();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);
			
			void Close() { m_Running = false; }

			inline Window& GetWindow() { return *m_Window; }
			inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer;  }

			inline static Application& Get() { return *s_Instance; }

			const ApplicationSpecification& GetSpecification() const { return m_Specification; }
		
			void SubmitToMainThread(const std::function<void()>& func);

			void ReloadImGui() { m_ReloadImGui = true; m_ImGuiIsReloaded = false; }
			bool ImGuiIsReloaded() { return m_ImGuiIsReloaded; }

		private:
			bool OnWindowClose(WindowCloseEvent& e);
			bool OnWindowResize(WindowResizeEvent& e);

			void ExecuteMainThreadQueue();
		private:
			ApplicationSpecification m_Specification;
			Scope<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			bool m_Minimized = false;
			LayerStack m_LayerStack;
			float m_LastFrameTime = 0.0f;

			std::vector<std::function<void()>> m_MainThreadQueue;
			std::mutex m_MainThreadQueueMutex;

			bool m_ReloadImGui = false;
			bool m_ImGuiIsReloaded = false;
		private:
			static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}
