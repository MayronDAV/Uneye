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
			static void DrawCombo(const std::string& label, const std::string& value, const Func& func)
			{
				ImGui::PushID(label.c_str());

				ImGui::Spacing();

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

			static void DrawTextArgs(const std::string& label, const std::string& fmt, ...);

			static bool DrawVec2Input(const std::string& label, glm::vec2& values, float resetValue = 0.0f);

			template<typename Func>
			static void DrawClickableText(const std::string& label, std::string& value, const Func& onClick)
			{
				ImGui::PushID(label.c_str());

				ImGui::Spacing();

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, m_ColumnWidth);
				ImGui::Text(label.c_str());
				ImGui::NextColumn();

				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

				ImVec2 textSize = ImGui::CalcTextSize(value.c_str());
				ImVec2 textPos = ImGui::GetCursorScreenPos();
				ImVec2 textEndPos = ImVec2(textPos.x + textSize.x, textPos.y + textSize.y);

				if (ImGui::Button("##button", { textSize.x + 20.0f, lineHeight }))
				{
					onClick();
				}

				ImGui::SetCursorScreenPos(ImVec2(textPos.x + (textSize.x - ImGui::CalcTextSize(value.c_str()).x) * 0.5f + 10.0f, textPos.y));
				ImGui::TextUnformatted(value.c_str());

				//if (ImGui::IsItemClicked()) {
				//	onClick();
				//}

				ImGui::PopStyleVar();
				ImGui::Columns(1);
				ImGui::PopID();
			}

		private:
			static float m_ColumnWidth;
			static float m_TextColumnWidth;
	};
}
