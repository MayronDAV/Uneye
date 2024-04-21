#include <Uneye.h>

class ExampleLayer : public Uneye::Layer
{
	public:
		ExampleLayer() : Layer("Example") {}

		void OnUpdate() override {
			UNEYE_INFO("ExampleLayer::Update");
		}

		void OnEvent(Uneye::Event& e) override {
			UNEYE_TRACE("{0}", e);
		}
};


class Sandbox : public Uneye::Application
{
	public:
		Sandbox() {
			PushLayer(new ExampleLayer());
		}
		~Sandbox() = default;
};




Uneye::Application* Uneye::CreateApplication()
{
	return new Sandbox();
}