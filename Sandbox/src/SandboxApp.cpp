#include <Uneye.h>

class ExampleLayer : public Uneye::Layer
{
	public:
		ExampleLayer() : Layer("Example") {}

		void OnUpdate() override {
			//UNEYE_INFO("ExampleLayer::Update");
			if (Uneye::Input::isKeyPressed(Uneye::Key::Tab))
				UNEYE_FATAL("Tab has pressed");
		}

		void OnEvent(Uneye::Event& e) override {
			//UNEYE_TRACE("{0}", e);
			if (e.GetEventType() == Uneye::EventType::KeyPressed)
			{
				Uneye::KeyPressedEvent& event = (Uneye::KeyPressedEvent&)e;
				//UNEYE_TRACE("{0}", static_cast<char>(event.GetKeyCode()));
				//if (event.GetKeyCode() == Uneye::Key::Tab)
				//	UNEYE_FATAL("Tab has pressed");
				//UNEYE_TRACE("{0}", (char)event.GetKeyCode());
			}
		}
};


class Sandbox : public Uneye::Application
{
	public:
		Sandbox() {
			PushLayer(new ExampleLayer());
			PushOverlay(new Uneye::ImGuiLayer() );
		}
		~Sandbox() = default;
};




Uneye::Application* Uneye::CreateApplication()
{
	return new Sandbox();
}