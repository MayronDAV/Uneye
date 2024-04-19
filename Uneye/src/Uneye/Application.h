#pragma once

#include "Core.h"

namespace uy {

	class UY_API Application
	{
		public:
			Application();
			virtual ~Application();

			virtual void Run();
	};


	// Defined by CLIENT
	Application* CreateApplication();
};

