#include <Uneye.h>
#include <Uneye/Core/EntryPoint.h>
#include "EditorLayer.h"



namespace Uneye
{
	class UneyeEditor : public Application
	{
	public:
		UneyeEditor()
			:Application("Uneye Editor")
		{
			PushLayer(new EditorLayer());
		}

		~UneyeEditor() = default;
	};

	Application* CreateApplication()
	{
		return new UneyeEditor();
	}
}
