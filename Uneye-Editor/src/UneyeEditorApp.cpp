#include <Uneye.h>
#include <Uneye/Core/EntryPoint.h>
#include "EditorLayer.h"



namespace Uneye
{
	class UneyeEditor : public Application
	{
	public:
		UneyeEditor(ApplicationCommandLineArgs args)
			: Application("Uneye-Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~UneyeEditor() = default;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new UneyeEditor(args);
	}
}
