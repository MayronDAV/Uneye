#include "ProjectPanelLayer.h"

#include "Uneye/Utils/PlatformUtils.h"


#include <imgui/imgui.h>


namespace Uneye
{
	bool ProjectPanelLayer::m_Running = true;

	ProjectPanelLayer::~ProjectPanelLayer()
	{
		m_Running = false;
	}

	void ProjectPanelLayer::OnAttach()
	{
		while (m_Running)
		{
			ImGui::SetNextWindowPos({ 0, 0 });
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			ImGui::Begin("##ProjectPanelLayer", 0, flags);

			if (ImGui::Button("Open a project..."))
			{
				std::string filepath = FileDialogs::OpenFile("Uneye Project (*.uyproj) | *.uyproj ");
				m_Running = false;
			}

			ImGui::End();
		}
	}

	void ProjectPanelLayer::OnImGuiRender()
	{
	}
}
