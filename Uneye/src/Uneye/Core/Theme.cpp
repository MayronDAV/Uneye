#include "uypch.h"
#include "Theme.h"



namespace Uneye
{
	Theme::Theme()
	{
		Path = std::string();
		Name = "Untitled";

		Alpha = 1.0f;
		WindowRounding = 7.0f;
		WindowBorderSize = 1.0f;
		ChildRounding = 0.0f;
		ChildBorderSize = 1.0f;
		PopupRounding = 0.0f;
		PopupBorderSize = 1.0f;
		FrameRounding = 0.0f;
		FrameBorderSize = 0.0f;
		IndentSpacing = 21.0f;
		ColumnsMinSpacing = 6.0f;
		ScrollbarSize = 16.0f;
		ScrollbarRounding = 9.0f;
		GrabMinSize = 10.0f;
		GrabRounding = 0.0f;
		TabRounding = 4.0f;
		TabBorderSize = 0.0f;
		MouseCursorScale = 1.0f;
		CurveTessellationTol = 1.25f;

		WindowPadding = { 8, 8 };
		WindowMinSize = { 32, 32 };
		WindowTitleAlign = { 0, 0.5f };
		FramePadding = { 4, 3 };
		ItemSpacing = { 8, 4 };
		ItemInnerSpacing = { 4, 4 };
		TouchExtraPadding = { 0, 0 };
		ButtonTextAlign = { 0.5f, 0.5f };
		SelectableTextAlign = { 0, 0 };
		DisplayWindowPadding = { 19, 19 };
		DisplaySafeAreaPadding = { 3, 3 };

		AntiAliasedLines = true;
		AntiAliasedFill = true;

		Colors = {
			{ "Text", glm::vec4(1.00f, 1.00f, 1.00f, 1.00f) },
			{ "TextDisabled", glm::vec4(0.50f, 0.50f, 0.50f, 1.00f) },
			{ "WindowBg", glm::vec4(0.06f, 0.06f, 0.06f, 0.94f) },
			{ "ChildBg", glm::vec4(0.00f, 0.00f, 0.00f, 0.00f) },
			{ "PopupBg", glm::vec4(0.08f, 0.08f, 0.08f, 0.94f) },
			{ "Border", glm::vec4(0.43f, 0.43f, 0.50f, 0.50f) },
			{ "BorderShadow", glm::vec4(0.00f, 0.00f, 0.00f, 0.00f) },
			{ "FrameBg", glm::vec4(0.16f, 0.29f, 0.48f, 0.54f) },
			{ "FrameBgHovered", glm::vec4(0.26f, 0.59f, 0.98f, 0.40f) },
			{ "FrameBgActive", glm::vec4(0.26f, 0.59f, 0.98f, 0.67f) },
			{ "TitleBg", glm::vec4(0.04f, 0.04f, 0.04f, 1.00f) },
			{ "TitleBgActive", glm::vec4(0.16f, 0.29f, 0.48f, 1.00f) },
			{ "TitleBgCollapsed", glm::vec4(0.00f, 0.00f, 0.00f, 0.51f) },
			{ "MenuBarBg", glm::vec4(0.14f, 0.14f, 0.14f, 1.00f) },
			{ "ScrollbarBg", glm::vec4(0.02f, 0.02f, 0.02f, 0.53f) },
			{ "ScrollbarGrab", glm::vec4(0.31f, 0.31f, 0.31f, 1.00f) },
			{ "ScrollbarGrabHovered", glm::vec4(0.41f, 0.41f, 0.41f, 1.00f) },
			{ "ScrollbarGrabActive", glm::vec4(0.51f, 0.51f, 0.51f, 1.00f) },
			{ "CheckMark", glm::vec4(0.26f, 0.59f, 0.98f, 1.00f) },
			{ "SliderGrab", glm::vec4(0.24f, 0.52f, 0.88f, 1.00f) },
			{ "SliderGrabActive", glm::vec4(0.26f, 0.59f, 0.98f, 1.00f) },
			{ "Button", glm::vec4(0.26f, 0.59f, 0.98f, 0.40f) },
			{ "ButtonHovered", glm::vec4(0.26f, 0.59f, 0.98f, 1.00f) },
			{ "ButtonActive", glm::vec4(0.06f, 0.53f, 0.98f, 1.00f) },
			{ "Header", glm::vec4(0.26f, 0.59f, 0.98f, 0.31f) },
			{ "HeaderHovered", glm::vec4(0.26f, 0.59f, 0.98f, 0.80f) },
			{ "HeaderActive", glm::vec4(0.26f, 0.59f, 0.98f, 1.00f) },
			{ "Separator", glm::vec4(0.43f, 0.43f, 0.50f, 0.50f) },
			{ "SeparatorHovered", glm::vec4(0.10f, 0.40f, 0.75f, 0.78f) },
			{ "SeparatorActive", glm::vec4(0.10f, 0.40f, 0.75f, 1.00f) },
			{ "ResizeGrip", glm::vec4(0.26f, 0.59f, 0.98f, 0.25f) },
			{ "ResizeGripHovered", glm::vec4(0.26f, 0.59f, 0.98f, 0.67f) },
			{ "ResizeGripActive", glm::vec4(0.26f, 0.59f, 0.98f, 0.95f) },
			{ "Tab", glm::vec4(0.26f, 0.59f, 0.98f, 0.31f) },
			{ "TabHovered", glm::vec4(0.26f, 0.59f, 0.98f, 0.80f) },
			{ "TabActive", glm::vec4(0.26f, 0.59f, 0.98f, 0.6f) },
			{ "TabUnfocused", glm::vec4(0.26f, 0.59f, 0.98f, 0.80f) },
			{ "TabUnfocusedActive", glm::vec4(0.26f, 0.59f, 0.98f, 0.40f) },
			{ "PlotLines", glm::vec4(0.61f, 0.61f, 0.61f, 1.00f) },
			{ "PlotLinesHovered", glm::vec4(1.00f, 0.43f, 0.35f, 1.00f) },
			{ "PlotHistogram", glm::vec4(0.90f, 0.70f, 0.00f, 1.00f) },
			{ "PlotHistogramHovered", glm::vec4(1.00f, 0.60f, 0.00f, 1.00f) },
			{ "TextSelectedBg", glm::vec4(0.26f, 0.59f, 0.98f, 0.35f) },
			{ "DragDropTarget", glm::vec4(1.00f, 1.00f, 0.00f, 0.90f) },
			{ "NavHighlight", glm::vec4(0.26f, 0.59f, 0.98f, 1.00f) },
			{ "NavWindowingHighlight", glm::vec4(1.00f, 1.00f, 1.00f, 0.70f) },
			{ "NavWindowingDimBg", glm::vec4(0.80f, 0.80f, 0.80f, 0.20f) },
			{ "ModalWindowDimBg", glm::vec4(0.80f, 0.80f, 0.80f, 0.35f) }
		};
	}
}
