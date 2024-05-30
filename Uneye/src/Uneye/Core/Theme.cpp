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
			{ "WindowBg", glm::vec4{0.1, 0.1, 0.13, 1} },
			{ "MenuBarBg", glm::vec4{0.16, 0.16, 0.21, 1} },
			{ "Border", glm::vec4{0.44, 0.37, 0.61, 0.29} },
			{ "BorderShadow", glm::vec4{0.0, 0.0, 0.0, 0.24} },
			{ "Text", glm::vec4{1, 1, 1, 1} },
			{ "TextDisabled", glm::vec4{0.5, 0.5, 0.5, 1} },
			{ "Header", glm::vec4{0.13, 0.13, 0.17, 1} },
			{ "HeaderHovered", glm::vec4{0.19, 0.2, 0.25, 1} },
			{ "HeaderActive", glm::vec4{0.16, 0.16, 0.21, 1} },
			{ "Button", glm::vec4{0.13, 0.13, 0.17, 1} },
			{ "ButtonHovered", glm::vec4{0.19, 0.2, 0.25, 1} },
			{ "ButtonActive", glm::vec4{0.16, 0.16, 0.21, 1} },
			{ "CheckMark", glm::vec4{0.74, 0.58, 0.98, 1} },
			{ "PopupBg", glm::vec4{0.1, 0.1, 0.13, 0.92} },
			{ "SliderGrab", glm::vec4{0.44, 0.37, 0.61, 0.54} },
			{ "SliderGrabActive", glm::vec4{0.74, 0.58, 0.98, 0.54} },
			{ "FrameBg", glm::vec4{0.13, 0.13, 0.17, 1.0} },
			{ "FrameBgHovered", glm::vec4{0.19, 0.2, 0.25, 1.0} },
			{ "FrameBgActive", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "Tab", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "TabHovered", glm::vec4{0.24, 0.24, 0.32, 1.0} },
			{ "TabActive", glm::vec4{0.2, 0.22, 0.27, 1.0} },
			{ "TabUnfocused", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "TabUnfocusedActive", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "TitleBg", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "TitleBgActive", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "TitleBgCollapsed", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "ScrollbarBg", glm::vec4{0.1, 0.1, 0.13, 1.0} },
			{ "ScrollbarGrab", glm::vec4{0.16, 0.16, 0.21, 1.0} },
			{ "ScrollbarGrabHovered", glm::vec4{0.19, 0.2, 0.25, 1.0} },
			{ "ScrollbarGrabActive", glm::vec4{0.24, 0.24, 0.32, 1.0} },
			{ "Separator", glm::vec4{0.44, 0.37, 0.61, 1.0} },
			{ "SeparatorHovered", glm::vec4{0.74, 0.58, 0.98, 1.0} },
			{ "SeparatorActive", glm::vec4{0.84, 0.58, 1.0, 1.0} },
			{ "ResizeGrip", glm::vec4{0.44, 0.37, 0.61, 0.29} },
			{ "ResizeGripHovered", glm::vec4{0.74, 0.58, 0.98, 0.29} },
			{ "ResizeGripActive", glm::vec4{0.84, 0.58, 1.0, 0.29} },
			{ "ChildBg", glm::vec4{0, 0, 0, 0} },
			{ "PlotLines", glm::vec4{0.61, 0.61, 0.61, 1} },
			{ "PlotLinesHovered", glm::vec4{1, 0.43, 0.35, 1} },
			{ "PlotHistogram", glm::vec4{0.9, 0.7, 0, 1} },
			{ "PlotHistogramHovered", glm::vec4{1, 0.6, 0, 1} },
			{ "TextSelectedBg", glm::vec4{0.26, 0.59, 0.98, 0.35} },
			{ "DragDropTarget", glm::vec4{1, 1, 0, 0.9} },
			{ "NavHighlight", glm::vec4{0.26, 0.59, 0.98, 1} },
			{ "NavWindowingHighlight", glm::vec4{1, 1, 1, 0.7} },
			{ "NavWindowingDimBg", glm::vec4{0.8, 0.8, 0.8, 0.2} },
			{ "ModalWindowDimBg", glm::vec4{0.8, 0.8, 0.8, 0.35} },
		};
	}
}
