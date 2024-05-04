#include <Uneye.h>
#include <Uneye/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include "Sandbox2D/Sandbox2D.h"
#include "example1/example1.h"




class Sandbox : public Uneye::Application
{
public:
	Sandbox()
	{
		//PushLayer(new Sandbox2D());
		PushLayer(new Example1());
	}

	~Sandbox() = default;

};

Uneye::Application* Uneye::CreateApplication()
{
	return new Sandbox();
}