#include <Uneye.h>




class Sandbox : public Uneye::Application
{
	public:
		Sandbox() { }
		~Sandbox() {}
};




Uneye::Application* Uneye::CreateApplication()
{
	return new Sandbox();
}