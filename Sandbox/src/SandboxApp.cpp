#include <Uneye.h>
#include <Uneye/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include "Sandbox2D/Sandbox2D.h"
#include "example1/example1.h"


class Sandbox : public Uneye::Application
{
	public:
		Sandbox(const Uneye::ApplicationSpecification& specification)
			: Uneye::Application(specification)
		{
			PushLayer(new Sandbox2D());

			//PushLayer(new Example1());
		}

		~Sandbox() = default;
};

Uneye::Application* Uneye::CreateApplication(Uneye::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Uneye-Editor";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
