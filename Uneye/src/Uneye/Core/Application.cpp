#include "uypch.h"
#include "Uneye/Core/Application.h"

#include "Uneye/Core/Log.h"

#include "Uneye/Renderer/Renderer.h"
#include "Uneye/Scripting/ScriptEngine.h"

#include "Uneye/Core/Input.h"
#include "Uneye/Renderer/Camera.h"
#include "Uneye/Utils/PlatformUtils.h"

#include "Uneye/Core/ThemeManager.h"

#include <filesystem>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>





namespace Uneye {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
#define BIND_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		UNEYE_PROFILE_FUNCTION();

		UNEYE_CORE_ASSERT(s_Instance, "Application already exists!");
		s_Instance = this;

		// Set working directory here
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_Window->SetVSync(false);

	}

	Application::~Application()
	{

		ScriptEngine::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		UNEYE_PROFILE_FUNCTION();

		layer->OnAttach();
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		UNEYE_PROFILE_FUNCTION();

		layer->OnAttach();
		m_LayerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& e)
	{
		UNEYE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
			{
				//UNEYE_CORE_TRACE("{0} -> {1}", (*it)->GetName(), e);
				break;
			}
		}
	}

	void Application::Run()
	{
		UNEYE_PROFILE_FUNCTION();

		while (m_Running)
		{
			UNEYE_PROFILE_SCOPE("RunLoop");

			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (m_ReloadImGui)
			{
				m_ImGuiLayer->OnDetach();
				m_LayerStack.PopOverlay(m_ImGuiLayer);
				m_ImGuiLayer = nullptr;

				m_ImGuiLayer = new ImGuiLayer();
				PushOverlay(m_ImGuiLayer);

				auto themeFunc = ThemeManager::GetTheme(m_Theme);
				if (themeFunc != NULL) themeFunc();

				m_ReloadImGui = false;
				m_ImGuiIsReloaded = true;
			}


			ExecuteMainThreadQueue();

			if (!m_Minimized)
			{

				{
					UNEYE_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					UNEYE_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();

			}

			m_Window->OnUpdate();
		}
	}

	void Application::SubmitToMainThread(const std::function<void()>& func)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(func);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		UNEYE_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::vector<std::function<void()>> copy;
		{
			std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
			copy = m_MainThreadQueue;
			m_MainThreadQueue.clear();
		}

		for (auto& func : copy)
			func();
	}


}
