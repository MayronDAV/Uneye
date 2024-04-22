#include "Uneyepch.h"

#include "WinWindow.h"

#include "Uneye/Log.h"

// Events
#include "Uneye/Events/Event.h"
#include "Uneye/Events/KeyEvent.h"
#include "Uneye/Events/MouseEvent.h"
#include "Uneye/Events/ApplicationEvent.h"



namespace Uneye
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		UNEYE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WinWindow(props);
	}

	WinWindow::WinWindow(const WindowProps& props) {
		Init(props);
	}

	WinWindow::~WinWindow() {
		Shutdown();
	}

	void WinWindow::Init(const WindowProps& props) {

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		UNEYE_CORE_TRACE("Creating a Window '{0}': {1}, {2}", m_Data.Title, m_Data.Width, m_Data.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();

			UNEYE_CORE_ASSERT(!success, "An Error has ocurred on glfwInit!");

			s_GLFWInitialized = success;
		}


		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		UNEYE_CORE_ASSERT(m_Window == nullptr, "An Error has ocurred on window create");
		UNEYE_CORE_INFO("Window '{0}' was created", m_Data.Title);

		glfwMakeContextCurrent(m_Window);

		// Load glad
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		UNEYE_CORE_ASSERT(!status, "Faile to initialize Glad!");

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set Callbacks
		glfwSetErrorCallback(GLFWErrorCallback);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, 
			int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);

		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, 
			int key, int scancode, int action, int mods) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
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
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}

		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window,
			int button, int action, int mods)
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

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, 
			double xoffset, double yoffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xoffset, (float)yoffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window,
			double xpos, double ypos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xpos, (float)ypos);
			data.EventCallback(event);
		});

	}

	void WinWindow::Shutdown() {
		UNEYE_CORE_WARN("Window '{0}' shutdown", m_Data.Title);
		glfwDestroyWindow(m_Window);
	}

	void WinWindow::OnUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WinWindow::SetVSync(bool enabled) {

		glfwSwapInterval((enabled) ? 1 : 0);

		m_Data.VSync = enabled;

		UNEYE_CORE_INFO("Vsync was set to '{0}'", enabled);
	}

	bool WinWindow::IsVSync() const{
		return m_Data.VSync;
	}
}