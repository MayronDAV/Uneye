#pragma once

#ifdef UNEYE_PLATFORM_WINDOWS

#include "Application.h"

extern Uneye::Application* Uneye::CreateApplication();

int main(int argc, char** argv)
{
	Uneye::Log::Init();
	UNEYE_CORE_WARN("Initialized!");
	int a = 10;
	UNEYE_INFO("Hello Var = {0}", a);
		
	auto app = Uneye::CreateApplication();
	app->Run();
	delete app;
	
	return 0;
}


#endif // UNEYE_PLATFORM_WINDOWS
