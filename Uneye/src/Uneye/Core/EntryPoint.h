#pragma once

#ifdef UNEYE_PLATFORM_WINDOWS

extern Uneye::Application* Uneye::CreateApplication();

int main(int argc, char** argv)
{
	Uneye::Log::Init();
	UNEYE_CORE_INFO("Initialized Application!");
	auto app = Uneye::CreateApplication();
	app->Run();
	delete app;
}

#endif