#include "uypch.h"
#include "WindowsWindow.h"

#include "Uneye/Events/ApplicationEvent.h"
#include "Uneye/Events/MouseEvent.h"
#include "Uneye/Events/KeyEvent.h"

#include "Uneye/Asset/TextureImporter.h"

#include "Uneye/Renderer/Renderer.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <random>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Platform/Windows/Resource/resource1.h"





namespace Uneye {
	
	namespace Utils
	{
		int CursorModeToGLFWCursorMode(CursorMode p_mode)
		{
			switch (p_mode)
			{
				case Uneye::CursorMode::HIDDEN:   return GLFW_CURSOR_HIDDEN;
				case Uneye::CursorMode::DISABLED: return GLFW_CURSOR_DISABLED;
				case Uneye::CursorMode::NORMAL:   return GLFW_CURSOR_NORMAL;
			}

			UNEYE_CORE_ERROR("Unknown cursor mode!");
			return 0;
		}
	}

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		UNEYE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	static void SetWindowIcon(GLFWwindow* window)
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);
		HICON hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_MYICON));

		if (hIcon)
		{
			HWND hwnd = glfwGetWin32Window(window);
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(hwnd, WM_SETICON, ICON_SMALL2, (LPARAM)hIcon);
		}
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

	void WindowsWindow::SetCursorMode(CursorMode p_mode)
	{
		int mode = Utils::CursorModeToGLFWCursorMode(p_mode);
		if (mode != 0)
			glfwSetInputMode(m_Window, GLFW_CURSOR, mode);
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

		SetWindowIcon(m_Window);

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

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

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

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
		});

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

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int pathCount, const char* paths[])
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			std::vector<std::filesystem::path> filepaths(pathCount);
			for (int i = 0; i < pathCount; i++)
				filepaths[i] = paths[i];

			WindowDropEvent event(std::move(filepaths));
			data.EventCallback(event);
		});
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

		glfwSwapInterval(enabled ? 1 : 0);

		m_Data.VSync = enabled;
		UNEYE_CORE_INFO("Vsync has setted to {0}", enabled);
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}
