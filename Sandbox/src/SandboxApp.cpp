#include <Uneye.h>

class Sandbox : public Uneye::Application
{
	public:
		Sandbox() { }
		~Sandbox() {}

		virtual void Run() {
			printf("Hello World!!\n");

			while (true)
			{

			}
		}
};

Uneye::Application* Uneye::CreateApplication()
{
	return new Sandbox();
}