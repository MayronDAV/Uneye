#include <Uneye.h>

#include "imgui/imgui.h"

class ExampleLayer : public Uneye::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Uneye::Input::IsKeyPressed(Uneye::Key::Tab))
			UNEYE_TRACE("Tab key is pressed (poll)!");
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Uneye::Event& event) override
	{
		if (event.GetEventType() == Uneye::EventType::KeyPressed)
		{
			Uneye::KeyPressedEvent& e = (Uneye::KeyPressedEvent&)event;
			if (e.GetKeyCode() == Uneye::Key::Tab)
				UNEYE_TRACE("Tab key is pressed (event)!");
			UNEYE_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

};

class Sandbox : public Uneye::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() = default;

};

Uneye::Application* Uneye::CreateApplication()
{
	return new Sandbox();
}