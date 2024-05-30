#pragma once

#include "Theme.h"

#include <string>
#include <unordered_map>
#include <functional>


namespace Uneye
{
	class ThemeManager
	{
		public:
			static const std::function<void()>& GetTheme(const std::string& name);
			static void SubmitTheme(const std::string& name, const std::function<void()>& func);
			static bool LoadTheme(const std::string& filepath);

			static const std::unordered_map<std::string, Theme>& GetThemes() { return s_Themes; }
		private:
			static std::function<void()> CreateThemeFunc(const Theme& theme);

			static std::unordered_map<std::string, Theme> s_Themes;
			static std::unordered_map<std::string, std::function<void()>> s_ThemesFunc;

	};

}
