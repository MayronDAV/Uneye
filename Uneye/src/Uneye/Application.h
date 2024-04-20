#pragma once

#include "Core.h"

namespace Uneye {

	class UNEYE_API Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();
	};


	// Defined by CLIENT
	Application* CreateApplication();
};

