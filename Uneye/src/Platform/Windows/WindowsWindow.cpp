#include "uypch.h"
#include "WindowsWindow.h"

#include "Uneye/Events/ApplicationEvent.h"
#include "Uneye/Events/MouseEvent.h"
#include "Uneye/Events/KeyEvent.h"

#include <glad/glad.h>
#include "Uneye/Renderer/Texture.h"
#include "Uneye/Renderer/Renderer.h"

#include <random>




namespace Uneye {
	
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		UNEYE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		UNEYE_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		UNEYE_PROFILE_FUNCTION();

		Shutdown();
		glfwTerminate();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		UNEYE_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_GLFWInitialized)
		{
			UNEYE_PROFILE_SCOPE("glfwInit");

			int success = glfwInit();
			UNEYE_CORE_ASSERT(!success, "Could not intialize GLFW!");
			UNEYE_CORE_INFO("GLFW has been initialized!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		{
			UNEYE_PROFILE_SCOPE("glfwCreateWindow");

	#ifdef UNEYE_DEBUG
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	#endif
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		}
		UNEYE_CORE_ASSERT(m_Window == nullptr, "An error has ocurred on window create");
		UNEYE_CORE_INFO("Window {0} was created with ({1}, {2})", props.Title, props.Width, props.Height);
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowSizeLimits(m_Window, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});
		UNEYE_CORE_INFO("Setted window size callback");

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});
		UNEYE_CORE_INFO("Setted window close callback");

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, false);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
			}
		});
		UNEYE_CORE_INFO("Setted key callback");

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
		});
		UNEYE_CORE_INFO("Setted char callback");

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});
		UNEYE_CORE_INFO("Setted mouse button callback");

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});
		UNEYE_CORE_INFO("Setted scroll callback");

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
		UNEYE_CORE_INFO("Setted cursor pos callback");
	}

	void WindowsWindow::Shutdown()
	{
		UNEYE_PROFILE_FUNCTION();

		UNEYE_CORE_WARN("Window {0} has shutdown", m_Data.Title);
		glfwDestroyWindow(m_Window);

		m_Window = nullptr;
		//m_Context = nullptr;
	}

	void WindowsWindow::OnUpdate()
	{
		UNEYE_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		UNEYE_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
		UNEYE_CORE_INFO("Vsync has setted to {0}", enabled);
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}
