#include "UI.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace Uneye
{
	float UI::m_ColumnWidth = 135.0f;
	float UI::m_TextColumnWidth = UI::m_ColumnWidth + 20.0f;

	bool UI::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, m_ColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, {0.6f, 0.1f, 0.15f, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.8f, 0.3f, 0.35f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.6f, 0.1f, 0.15f, 1 });
		bool buttonX = ImGui::Button("X", buttonSize);
		if (buttonX)
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		bool dragX = ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.6f, 0.15f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.35f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.6f, 0.15f, 1 });
		bool buttonY = ImGui::Button("Y", buttonSize);
		if (buttonY)
			values.y = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		bool dragY = ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.15f, 0.6f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.35f, 0.8f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.15f, 0.6f, 1 });
		bool buttonZ = ImGui::Button("Z", buttonSize);
		if (buttonZ)
			values.z = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		bool dragZ = ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();


		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return buttonX || buttonY || buttonZ || dragX || dragY || dragZ;
	}


	bool UI::DrawFloatControl(const std::string& label, float& value, float resetValue)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, m_ColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::Text("");

		ImGui::SameLine();
		bool result = ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopStyleVar();


		ImGui::Columns(1);
		ImGui::PopID();
		return result;
	}


	bool UI::DrawInputText(const std::string& label, char* buffer, size_t buffer_size)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, m_TextColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::Text("");

		ImGui::SameLine();
		bool result = ImGui::InputText("##Label", buffer, buffer_size);
		ImGui::PopStyleVar();

		ImGui::Columns(1);


		ImGui::PopID();

		return result;
	}
	bool UI::DrawCheckBox(const std::string& label, bool* value)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, m_ColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::Text("");

		ImGui::SameLine();
		bool result = ImGui::Checkbox("##Label", value);
		ImGui::PopStyleVar();

		ImGui::Columns(1);


		ImGui::PopID();

		return result;
	}
	bool UI::DrawColorEdit4(const std::string& label, glm::vec4& values, float resetValue)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, m_ColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::Text("");

		ImGui::SameLine();
		bool result = ImGui::ColorEdit4("##RGBA", glm::value_ptr(values));


		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return result;
	}

	void UI::DrawTextArgs(const std::string& label, const std::string& fmt, ...)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, m_TextColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::Text("");

		ImGui::SameLine();
		va_list args;
		va_start(args, fmt.c_str());
		ImGui::TextV(fmt.c_str(), args);
		va_end(args);


		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

}