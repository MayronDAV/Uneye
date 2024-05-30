#pragma once
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>


namespace Uneye
{
	struct Theme
	{
		Theme();
		~Theme() = default;

		std::string Path;
		std::string Name;

		float Alpha;
		float WindowRounding;
		float WindowBorderSize;
		float ChildRounding;
		float ChildBorderSize;
		float PopupRounding;
		float PopupBorderSize;
		float FrameRounding;
		float FrameBorderSize;
		float IndentSpacing;
		float ColumnsMinSpacing;
		float ScrollbarSize;
		float ScrollbarRounding;
		float GrabMinSize;
		float GrabRounding;
		float TabRounding;
		float TabBorderSize;
		float MouseCursorScale;
		float CurveTessellationTol;

		glm::vec2 WindowPadding;
		glm::vec2 WindowMinSize;
		glm::vec2 WindowTitleAlign;
		glm::vec2 FramePadding;
		glm::vec2 ItemSpacing;
		glm::vec2 ItemInnerSpacing;
		glm::vec2 TouchExtraPadding;
		glm::vec2 ButtonTextAlign;
		glm::vec2 SelectableTextAlign;
		glm::vec2 DisplayWindowPadding;
		glm::vec2 DisplaySafeAreaPadding;

		bool AntiAliasedLines;
		bool AntiAliasedFill;

		std::unordered_map<std::string, glm::vec4> Colors;
	};
}
