#pragma once

#ifdef UNEYE_PLATFORM_WINDOWS

extern Uneye::Application* Uneye::CreateApplication();

int main(int argc, char** argv)
{
	Uneye::Log::Init();
	UNEYE_CORE_WARN("Initialized Log!");
	int a = 5;
	UNEYE_INFO("Hello! Var={0}", a);

	auto app = Uneye::CreateApplication();
	app->Run();
	delete app;
}

#endif