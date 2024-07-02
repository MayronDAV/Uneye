#pragma once

#include "Uneye/Core/Window.h"

#include <GLFW/glfw3.h>

#include "Platform/OpenGL/OpenGLContext.h"





namespace Uneye {

	class WindowsWindow : public Window
	{
		public:
			WindowsWindow(const WindowProps& props);
			virtual ~WindowsWindow();

			void OnUpdate() override;

			inline unsigned int GetWidth() const override { return m_Data.Width; }
			inline unsigned int GetHeight() const override { return m_Data.Height; }

			// Window attributes
			inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
			void SetVSync(bool enabled) override;
			bool IsVSync() const override;

			inline virtual void* GetNativeWindow() const override { return m_Window; }

			virtual void SetCursorMode(CursorMode p_mode) override;

		private:
			virtual void Init(const WindowProps& props);
			virtual void Shutdown();

		private:
			GLFWwindow* m_Window = nullptr;
			GraphicsContext* m_Context = nullptr;

			struct WindowData
			{
				std::string Title;
				unsigned int Width;
				unsigned int Height;
				bool VSync;

				EventCallbackFn EventCallback;
			};

			WindowData m_Data;
	};

}
