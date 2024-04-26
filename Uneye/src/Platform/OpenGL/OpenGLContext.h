#pragma once

#include "Uneye/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Uneye
{
	class OpenGLContext : public GraphicsContext
	{
		public:
			OpenGLContext(GLFWwindow* windowhandle);

			virtual void Init() override;
			virtual void SwapBuffers() override;

		private:
			GLFWwindow* m_WindowHandle = nullptr;
	};

}
