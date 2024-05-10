#include "uypch.h"
#include "Uneye/Core/FontManager.h"

#include <imgui.h>
#include <imgui_internal.h>



namespace Uneye
{
	std::unordered_map<std::string, ImFont*> FontManager::s_Fonts = {};



	ImFont* FontManager::GetFont(const std::string& name)
	{
		auto it = s_Fonts.find(name);
		UNEYE_CORE_ASSERT(it == s_Fonts.end(), "Unknown name!");

		return s_Fonts[name];
	}

	void FontManager::PushFont(const std::string& name, ImFont* font)
	{
		s_Fonts[name] = font;
	}

	void FontManager::PopFont(const std::string& name)
	{
		auto it = s_Fonts.find(name);
		UNEYE_CORE_ASSERT(it == s_Fonts.end(), "Unknown name!");

		s_Fonts.erase(name);
	}

	void FontManager::SetDefaultFont(const std::string& name)
	{
		auto it = s_Fonts.find(name);
		UNEYE_CORE_ASSERT(it == s_Fonts.end(), "Unknown name!");

		ImGuiIO& io = ImGui::GetIO();

		io.FontDefault = s_Fonts[name];
	}

	void FontManager::SetDefaultFont(const std::string& name, ImFont* font)
	{
		PushFont(name, font);

		SetDefaultFont(name);
	}

}