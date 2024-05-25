#include <Uneye.h>
#include <Uneye/Core/EntryPoint.h>
#include "EditorLayer.h"



namespace Uneye
{
	class UneyeEditor : public Application
	{
	public:
		UneyeEditor(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}

		~UneyeEditor() = default;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Uneye-Editor";
		spec.CommandLineArgs = args;

		//Project::New()->SaveActive("SandboxProject/Sandbox.uyproj");

		return new UneyeEditor(spec);
	}
}
