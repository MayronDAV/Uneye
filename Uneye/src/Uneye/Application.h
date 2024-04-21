#pragma once


#include "Uneyepch.h"

#include "Core.h"
#include "Uneye/Window.h"


namespace Uneye {

	class UNEYE_API Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();

		private:
			std::unique_ptr<Window> m_Window;
			bool m_Running = true;
	};


	// Defined by CLIENT
	Application* CreateApplication();
};

