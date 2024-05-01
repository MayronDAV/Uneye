#pragma once

#ifdef UNEYE_PLATFORM_WINDOWS

extern Uneye::Application* Uneye::CreateApplication();

int main(int argc, char** argv)
{
	Uneye::Log::Init();
	UNEYE_CORE_INFO("Initialized Application!");

	UNEYE_PROFILE_BEGIN_SESSION("Startup", "UneyeStartup.json");
	auto app = Uneye::CreateApplication();
	UNEYE_PROFILE_END_SESSION();

	UNEYE_PROFILE_BEGIN_SESSION("Runtime", "UneyeRuntime.json");
	app->Run();
	UNEYE_PROFILE_END_SESSION();

	UNEYE_PROFILE_BEGIN_SESSION("Shutdown", "UneyeShutdown.json");
	delete app;
	UNEYE_PROFILE_END_SESSION();
}

#endif