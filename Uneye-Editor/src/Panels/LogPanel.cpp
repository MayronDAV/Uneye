#include "LogPanel.h"

#include "Uneye/Core/Log.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Uneye
{
	namespace Utils
	{
		static std::string LogTypeToString(LogType p_type)
		{
			switch (p_type)
			{
				case Uneye::LogType::Trace:	   return "Trace";
				case Uneye::LogType::Info:	   return "Info";
				case Uneye::LogType::Warn:	   return "Warn";
				case Uneye::LogType::Error:	   return "Error";
				case Uneye::LogType::Critical: return "Critical";
			}

			return "Trace";
		}

		static glm::vec4 LogTypeColor(LogType p_type)
		{
			switch (p_type)
			{
				case Uneye::LogType::Trace:	   return { 0.5f, 0.5f, 0.5f, 1.0f };
				case Uneye::LogType::Info:	   return { 0.2f, 0.8f, 0.3f, 1.0f };
				case Uneye::LogType::Warn:	   return { 0.7f, 0.6f, 0.2f, 1.0f };
				case Uneye::LogType::Error:	   return { 0.8f, 0.2f, 0.3f, 1.0f };
				case Uneye::LogType::Critical: return { 1.0f, 0.4f, 0.5f, 1.0f };
			}

			return { 0.5f, 0.5f, 0.5f, 1.0f };
		}
	}

	LogPanel::LogPanel()
	{
	}

	void LogPanel::OnImGuiRender()
	{
		ImGui::Begin("Log");

		//UNEYE_INFO("DESENHANDO");

		/*if (ImGui::BeginTable("##LogTable", 2))

		for (auto [type, message] : Log::GetLoggerMessage()->GetMessages())
		{
			ImGui::Text("{}", Utils::LogTypeToString(type));
			ImGui::NextColumn();
			ImGui::Text("{}", message);
			ImGui::NextColumn();
		}

		ImGui::EndTable();*/

		if (ImGui::BeginTable("##LogTabel", 2, ImGuiTableFlags_BordersInner))
		{
			ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 50.0f);
			ImGui::TableSetupColumn("Message");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();

			for (const auto msg : Log::GetLoggerMessage()->GetMessages())
			{

				ImGui::PushStyleColor(ImGuiCol_Text, { Utils::LogTypeColor(msg.Type).x, Utils::LogTypeColor(msg.Type).y, Utils::LogTypeColor(msg.Type).z, Utils::LogTypeColor(msg.Type).w });
				ImGui::TableSetColumnIndex(0);

				ImGui::Text(Utils::LogTypeToString(msg.Type).c_str());

				ImGui::TableSetColumnIndex(1);

				ImGui::Text(msg.Message.c_str());

				ImGui::PopStyleColor();

				ImGui::TableNextRow();
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}
}
