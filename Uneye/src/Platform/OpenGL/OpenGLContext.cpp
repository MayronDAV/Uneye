#include "uypch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>



namespace Uneye
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowhandle)
		:m_WindowHandle(windowhandle)
	{
		UNEYE_CORE_ASSERT(!m_WindowHandle, "Window Handle is null, did you call new OpenGLContext() with your window?");
		UNEYE_CORE_INFO("OpenGLContext has Initialized!!!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		UNEYE_CORE_ASSERT(!status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}