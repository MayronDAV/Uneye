#pragma once

#include <string>
#include <glm/glm.hpp>



namespace Uneye
{
	class UI
	{
		public:
			static void SetColumnWidth(float w) { m_ColumnWidth = w; }
			static void SetTextColumnWidth(float w) { m_TextColumnWidth = w; }


			static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f);

			static bool DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f);

			static bool DrawInputText(const std::string& label, char* buffer, size_t buffer_size);

			static bool DrawCheckBox(const std::string& label, bool* value);

			static bool DrawColorEdit4(const std::string& label, glm::vec4& values, float resetValue = 0.0f);

			template<typename Func>
			static void DrawCombo(const std::string& label, const std::string& value, const Func& func);

			static void DrawTextArgs(const std::string& label, const std::string& fmt, ...);

		private:
			static float m_ColumnWidth;
			static float m_TextColumnWidth;
	};

	template<typename Func>
	inline void UI::DrawCombo(const std::string& label, const std::string& value, const Func& func)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, m_ColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::Text("");

		ImGui::SameLine();
		if (ImGui::BeginCombo("##Combo", value.c_str()))
		{

			func();

			ImGui::EndCombo();
		}


		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}
}