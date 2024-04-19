#pragma once

#ifdef UY_PLATFORM_WINDOWS

#include "Application.h"

extern uy::Application* uy::CreateApplication();

int main(int argc, char** argv)
{
	auto app = uy::CreateApplication();
	app->Run();
	delete app;

	return 0;
}


#endif // UY_PLATFORM_WINDOWS
