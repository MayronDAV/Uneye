#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Uneye/Core/FontManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
	

namespace Uneye
{
	class UI
	{
		public:
			static void SetColumnWidth(float w) { m_ColumnWidth = w; }
			static void SetTextColumnWidth(float w) { m_TextColumnWidth = w; }


			static bool DrawFloat3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f);

			static bool DrawFloat2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f);

			static bool DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f,
				float v_min = 0.0f, float v_max = 0.0f, float v_speed = 0.1f);

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

			static bool DrawFloat2Input(const std::string& label, glm::vec2& values, float resetValue = 0.0f);

			template<typename Func1, typename Func2>
			static void DrawClickableText(const std::string& label, std::string& value, const Func1& onResetButton, const Func2& onClick)
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

				//ImGui::PushFont(FontManager::GetFont("Arrows"));
				if (ImGui::Button("X", { lineHeight, lineHeight } ))
				{
					onResetButton();
				}
				//ImGui::PopFont();

				ImGui::SameLine();

				if (ImGui::Button("##button", { textSize.x + 20.0f, lineHeight }))
				{
					onClick();
				}

				ImGui::SetCursorScreenPos(ImVec2(textPos.x + (textSize.x - ImGui::CalcTextSize(value.c_str()).x) * 0.5f + 10.0f + lineHeight, textPos.y));
				ImGui::TextUnformatted(value.c_str());

				ImGui::PopStyleVar();
				ImGui::Columns(1);
				ImGui::PopID();
			}

			template<typename Func1, typename Func2, typename Func3>
			static void DrawClickableText(const std::string& label, std::string& value, const Func1& onResetButton, const Func2& onClick, const Func3& other)
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

				//ImGui::PushFont(FontManager::GetFont("Arrows"));
				if (ImGui::Button("X", { lineHeight, lineHeight }))
				{
					onResetButton();
				}
				//ImGui::PopFont();

				ImGui::SameLine();

				if (ImGui::Button("##button", { textSize.x + 20.0f, lineHeight }))
				{
					onClick();
				}

				ImGui::SetCursorScreenPos(ImVec2(textPos.x + (textSize.x - ImGui::CalcTextSize(value.c_str()).x) * 0.5f + 10.0f + lineHeight, textPos.y));
				ImGui::TextUnformatted(value.c_str());
				
				other();

				ImGui::PopStyleVar();
				ImGui::Columns(1);
				ImGui::PopID();
			}

		private:
			static float m_ColumnWidth;
			static float m_TextColumnWidth;
	};
}
