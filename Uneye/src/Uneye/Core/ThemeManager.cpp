#include "uypch.h"
#include "ThemeManager.h"

#include <yaml-cpp/yaml.h>

#include <imgui.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}


namespace Uneye
{
	namespace
	{
		#define BIND_MEMBER(Name, type) {\
			if (theme[#Name]) classTheme.Name = theme[#Name].as<type>();\
		}

		#define BIND_MEMBER_MAP(Name) {\
			if (theme["Colors"][#Name]) classTheme.Colors[#Name] = theme["Colors"][#Name].as<glm::vec4>();\
		}

		#define BIND_COLOR_IMGUI(Name) {\
			auto it = theme.Colors.find(#Name);\
			if (it != theme.Colors.end())\
			{\
				colors[ImGuiCol_##Name] = ImVec4(\
					it->second.r, it->second.g, it->second.b, it->second.a\
				);\
			}\
		 }

		#define BIND_STYLE_IMGUI(Name) {\
			style.Name = theme.Name;\
		}

		#define BIND_STYLE_VEC2_IMGUI(Name) {\
			style.Name = ImVec2(theme.Name.x, theme.Name.y);\
		}
	}

	std::unordered_map<std::string, Theme> ThemeManager::s_Themes;
	std::unordered_map<std::string, std::function<void()>> ThemeManager::s_ThemesFunc;


	const std::function<void()>& ThemeManager::GetTheme(const std::string& name)
	{
		auto it = s_ThemesFunc.find(name);
		if (it == s_ThemesFunc.end())
		{
			UNEYE_CORE_WARN("This theme not exists!!!");
			return NULL;
		}

		return it->second;
	}
	void ThemeManager::SubmitTheme(const std::string& name, const std::function<void()>& func)
	{
		if (s_ThemesFunc.find(name) != s_ThemesFunc.end())
			UNEYE_CORE_WARN("This theme already exists!!!");

		s_ThemesFunc[name] = func;
	}

	bool ThemeManager::LoadTheme(const std::string& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			UNEYE_CORE_ERROR("Failed to load .uythm file '{0}'\n     {1}", filepath, e.what());
			return false;
		}
		auto theme = data["Theme"];
		if (!theme)
			return false;

		Theme classTheme;

		std::string themeName = theme["Name"].as<std::string>();
		UNEYE_CORE_TRACE("Deserializing theme '{0}'", themeName);

		classTheme.Path = filepath;
		classTheme.Name = themeName;

		BIND_MEMBER(Alpha, float);
		BIND_MEMBER(WindowRounding, float);
		BIND_MEMBER(WindowBorderSize, float);
		BIND_MEMBER(ChildRounding, float);
		BIND_MEMBER(ChildBorderSize, float);
		BIND_MEMBER(PopupRounding, float);
		BIND_MEMBER(PopupBorderSize, float);
		BIND_MEMBER(FrameRounding, float);
		BIND_MEMBER(FrameBorderSize, float);
		BIND_MEMBER(IndentSpacing, float);
		BIND_MEMBER(ColumnsMinSpacing, float);
		BIND_MEMBER(ScrollbarSize, float);
		BIND_MEMBER(ScrollbarRounding, float);
		BIND_MEMBER(GrabMinSize, float);
		BIND_MEMBER(GrabRounding, float);
		BIND_MEMBER(TabRounding, float);
		BIND_MEMBER(TabBorderSize, float);
		BIND_MEMBER(MouseCursorScale, float);
		BIND_MEMBER(CurveTessellationTol, float);

		BIND_MEMBER(WindowPadding, glm::vec2);
		BIND_MEMBER(WindowMinSize, glm::vec2);
		BIND_MEMBER(WindowTitleAlign, glm::vec2);
		BIND_MEMBER(FramePadding, glm::vec2);
		BIND_MEMBER(ItemSpacing, glm::vec2);
		BIND_MEMBER(ItemInnerSpacing, glm::vec2);
		BIND_MEMBER(TouchExtraPadding, glm::vec2);
		BIND_MEMBER(ButtonTextAlign, glm::vec2);
		BIND_MEMBER(SelectableTextAlign, glm::vec2);
		BIND_MEMBER(DisplayWindowPadding, glm::vec2);
		BIND_MEMBER(DisplaySafeAreaPadding, glm::vec2);

		BIND_MEMBER(AntiAliasedLines, bool);
		BIND_MEMBER(AntiAliasedFill, bool);

		if (theme["Colors"])
		{

			BIND_MEMBER_MAP(WindowBg);
			BIND_MEMBER_MAP(MenuBarBg);
			BIND_MEMBER_MAP(Border);
			BIND_MEMBER_MAP(BorderShadow);
			BIND_MEMBER_MAP(Text);
			BIND_MEMBER_MAP(TextDisabled);
			BIND_MEMBER_MAP(Header);
			BIND_MEMBER_MAP(HeaderHovered);
			BIND_MEMBER_MAP(HeaderActive);
			BIND_MEMBER_MAP(ButtonBg);
			BIND_MEMBER_MAP(ButtonHovered);
			BIND_MEMBER_MAP(ButtonActive);
			BIND_MEMBER_MAP(CheckMark);
			BIND_MEMBER_MAP(PopupBg);
			BIND_MEMBER_MAP(SliderGrab);
			BIND_MEMBER_MAP(SliderGrabActive);
			BIND_MEMBER_MAP(FrameBg);
			BIND_MEMBER_MAP(FrameBgHovered);
			BIND_MEMBER_MAP(FrameBgActive);
			BIND_MEMBER_MAP(Tab);
			BIND_MEMBER_MAP(TabHovered);
			BIND_MEMBER_MAP(TabActive);
			BIND_MEMBER_MAP(TabUnfocused);
			BIND_MEMBER_MAP(TabUnfocusedActive);
			BIND_MEMBER_MAP(TitleBg);
			BIND_MEMBER_MAP(TitleBgActive);
			BIND_MEMBER_MAP(TitleBgCollapsed);
			BIND_MEMBER_MAP(ScrollbarBg);
			BIND_MEMBER_MAP(ScrollbarGrab);
			BIND_MEMBER_MAP(ScrollbarGrabHovered);
			BIND_MEMBER_MAP(ScrollbarGrabActive);
			BIND_MEMBER_MAP(Separator);
			BIND_MEMBER_MAP(SeparatorHovered);
			BIND_MEMBER_MAP(SeparatorActive);
			BIND_MEMBER_MAP(ResizeGrip);
			BIND_MEMBER_MAP(ResizeGripHovered);
			BIND_MEMBER_MAP(ResizeGripActive);
			BIND_MEMBER_MAP(ChildBg);
			BIND_MEMBER_MAP(PlotLines);
			BIND_MEMBER_MAP(PlotLinesHovered);
			BIND_MEMBER_MAP(PlotHistogram);
			BIND_MEMBER_MAP(PlotHistogramHovered);
			BIND_MEMBER_MAP(TextSelectedBg);
			BIND_MEMBER_MAP(DragDropTarget);
			BIND_MEMBER_MAP(NavHighlight);
			BIND_MEMBER_MAP(NavWindowingHighlight);
			BIND_MEMBER_MAP(NavWindowingDimBg);
			BIND_MEMBER_MAP(ModalWindowDimBg);
		}

		s_Themes[classTheme.Name] = classTheme;
		s_ThemesFunc[classTheme.Name] = CreateThemeFunc(classTheme);

		return true;
	}


	std::function<void()> ThemeManager::CreateThemeFunc(const Theme& theme)
	{
		UNEYE_PROFILE_FUNCTION();


		std::function<void()> func = [=]() {
			UNEYE_CORE_TRACE("Setting {} theme", theme.Name);

			auto& colors = ImGui::GetStyle().Colors;

			BIND_COLOR_IMGUI(WindowBg);
			BIND_COLOR_IMGUI(MenuBarBg);
			BIND_COLOR_IMGUI(Border);
			BIND_COLOR_IMGUI(BorderShadow);
			BIND_COLOR_IMGUI(Text);
			BIND_COLOR_IMGUI(TextDisabled);
			BIND_COLOR_IMGUI(Header);
			BIND_COLOR_IMGUI(HeaderHovered);
			BIND_COLOR_IMGUI(HeaderActive);
			BIND_COLOR_IMGUI(Button);
			BIND_COLOR_IMGUI(ButtonHovered);
			BIND_COLOR_IMGUI(ButtonActive);
			BIND_COLOR_IMGUI(CheckMark);
			BIND_COLOR_IMGUI(PopupBg);
			BIND_COLOR_IMGUI(SliderGrab);
			BIND_COLOR_IMGUI(SliderGrabActive);
			BIND_COLOR_IMGUI(FrameBg);
			BIND_COLOR_IMGUI(FrameBgHovered);
			BIND_COLOR_IMGUI(FrameBgActive);
			BIND_COLOR_IMGUI(Tab);
			BIND_COLOR_IMGUI(TabHovered);
			BIND_COLOR_IMGUI(TabActive);
			BIND_COLOR_IMGUI(TabUnfocused);
			BIND_COLOR_IMGUI(TabUnfocusedActive);
			BIND_COLOR_IMGUI(TitleBg);
			BIND_COLOR_IMGUI(TitleBgActive);
			BIND_COLOR_IMGUI(TitleBgCollapsed);
			BIND_COLOR_IMGUI(ScrollbarBg);
			BIND_COLOR_IMGUI(ScrollbarGrab);
			BIND_COLOR_IMGUI(ScrollbarGrabHovered);
			BIND_COLOR_IMGUI(ScrollbarGrabActive);
			BIND_COLOR_IMGUI(Separator);
			BIND_COLOR_IMGUI(SeparatorHovered);
			BIND_COLOR_IMGUI(SeparatorActive);
			BIND_COLOR_IMGUI(ResizeGrip);
			BIND_COLOR_IMGUI(ResizeGripHovered);
			BIND_COLOR_IMGUI(ResizeGripActive);
			BIND_COLOR_IMGUI(ChildBg);
			BIND_COLOR_IMGUI(PlotLines);
			BIND_COLOR_IMGUI(PlotLinesHovered);
			BIND_COLOR_IMGUI(PlotHistogram);
			BIND_COLOR_IMGUI(PlotHistogramHovered);
			BIND_COLOR_IMGUI(TextSelectedBg);
			BIND_COLOR_IMGUI(DragDropTarget);
			BIND_COLOR_IMGUI(NavHighlight);
			BIND_COLOR_IMGUI(NavWindowingHighlight);
			BIND_COLOR_IMGUI(NavWindowingDimBg);
			BIND_COLOR_IMGUI(ModalWindowDimBg);

			auto& style = ImGui::GetStyle();

			BIND_STYLE_IMGUI(Alpha);
			BIND_STYLE_IMGUI(WindowRounding);
			BIND_STYLE_IMGUI(WindowBorderSize);
			BIND_STYLE_IMGUI(ChildRounding);
			BIND_STYLE_IMGUI(ChildBorderSize);
			BIND_STYLE_IMGUI(PopupRounding);
			BIND_STYLE_IMGUI(PopupBorderSize);
			BIND_STYLE_IMGUI(FrameRounding);
			BIND_STYLE_IMGUI(FrameBorderSize);
			BIND_STYLE_IMGUI(IndentSpacing);
			BIND_STYLE_IMGUI(ColumnsMinSpacing);
			BIND_STYLE_IMGUI(ScrollbarSize);
			BIND_STYLE_IMGUI(ScrollbarRounding);
			BIND_STYLE_IMGUI(GrabMinSize);
			BIND_STYLE_IMGUI(GrabRounding);
			BIND_STYLE_IMGUI(TabRounding);
			BIND_STYLE_IMGUI(TabBorderSize);
			BIND_STYLE_IMGUI(MouseCursorScale);
			BIND_STYLE_IMGUI(CurveTessellationTol);

			BIND_STYLE_VEC2_IMGUI(WindowPadding);
			BIND_STYLE_VEC2_IMGUI(WindowMinSize);
			BIND_STYLE_VEC2_IMGUI(WindowTitleAlign);
			BIND_STYLE_VEC2_IMGUI(FramePadding);
			BIND_STYLE_VEC2_IMGUI(ItemSpacing);
			BIND_STYLE_VEC2_IMGUI(ItemInnerSpacing);
			BIND_STYLE_VEC2_IMGUI(TouchExtraPadding);
			BIND_STYLE_VEC2_IMGUI(ButtonTextAlign);
			BIND_STYLE_VEC2_IMGUI(SelectableTextAlign);
			BIND_STYLE_VEC2_IMGUI(DisplayWindowPadding);
			BIND_STYLE_VEC2_IMGUI(DisplaySafeAreaPadding);

			BIND_STYLE_IMGUI(AntiAliasedLines);
			BIND_STYLE_IMGUI(AntiAliasedFill);
			};

		return func;
	}
}
