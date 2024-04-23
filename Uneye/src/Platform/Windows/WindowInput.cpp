#include "Uneyepch.h"
#include "WindowInput.h"

#include "Uneye/Application.h"

#include <GLFW/glfw3.h>





namespace Uneye
{
	Input* Input::s_Instance = new WindowInput();

	bool WindowInput::isKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow()
			);
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowInput::isMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow()
			);
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow()
			);
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return std::pair<float, float>((float)xpos, (float)ypos);
	}

	float WindowInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return x;
	}

	float WindowInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return y;
	}

};
