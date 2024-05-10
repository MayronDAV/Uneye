#pragma once

#include <string>
#include <unordered_map>


struct ImFont;

namespace Uneye
{

	class FontManager
	{
		public:

			static ImFont* GetFont(const std::string& name);

			static void PushFont(const std::string& name, ImFont* font);
			static void PopFont(const std::string& name);

			static void SetDefaultFont(const std::string& name);
			static void SetDefaultFont(const std::string& name, ImFont* font);

		private:
			static std::unordered_map<std::string, ImFont*> s_Fonts;
	};


}