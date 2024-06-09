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

	bool first = true;

	void LogPanel::OnImGuiRender()
	{

		ImGui::Begin("Log");

		if (ImGui::BeginTable("##LogTabel", 2, ImGuiTableFlags_BordersInner))
		{
			const auto& messages = Log::GetLoggerMessage()->GetMessages();

			ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 50.0f);
			ImGui::TableSetupColumn("Message");
			ImGui::TableHeadersRow();

			ImGuiListClipper clipper;
			clipper.Begin(messages.size());
			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
				{
					auto msg = messages.at(row);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					auto color = Utils::LogTypeColor(msg.Type);
					ImGui::PushStyleColor(ImGuiCol_Text, { color.x, color.y, color.z, color.w });

					ImGui::Text(Utils::LogTypeToString(msg.Type).c_str());

					ImGui::TableSetColumnIndex(1);

					ImGui::Text(msg.Message.c_str());

					ImGui::PopStyleColor();
				}
			}

			ImGui::EndTable();
		}




		ImGui::End();
	}
}
