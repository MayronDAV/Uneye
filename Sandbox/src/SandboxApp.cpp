#include <Uneye.h>

class Sandbox : public uy::Application
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

uy::Application* uy::CreateApplication()
{
	return new Sandbox();
}