#include "AssetImporterPanel.h"
#include "Uneye/Core/Base.h"

#include "UI/UI.h"
#include "Uneye/Utils/PlatformUtils.h"

#include "Uneye/Asset/EditorAssetManager.h"

#include <imgui/imgui.h>


namespace Uneye
{
	AssetImporterPanel* AssetImporterPanel::s_Instance = nullptr;

	AssetImporterPanel::AssetImporterPanel()
	{
		//UNEYE_CORE_ASSERT(s_Instance != nullptr);
		if (s_Instance)
			s_Instance->Close();

		s_Instance = this;

		m_IsOpen = true;
	}

	void AssetImporterPanel::Close()
	{
		m_IsOpen = false;
		m_WasOpen = m_IsOpen;

		m_Options.FilePath = "";
		m_Options.Type = AssetType::None;
		m_Options.TileSize = { 1, 1 };
		m_Options.TileCoord = { 0, 0 };
		m_Options.SpriteSize = { 1, 1 };
	}

	void AssetImporterPanel::Open()
	{
		if (m_IsOpen)
		{
			ImGui::Begin("Asset Importer", &m_IsOpen);

			static std::string filepath = "FilePath";
			UI::DrawClickableText("FilePath", filepath, [&]() { filepath = "FilePath"; }, [&]() {

				filepath = FileDialogs::OpenFile("All files (*.*)");
				if (filepath.empty()) filepath = "FilePath";

				});
			if (!filepath.empty() && filepath != "FilePath")
				m_Options.FilePath = filepath;

			//filepath = std::filesystem::path(filepath).filename().string();

			const char* AssetTypeString[] = { "None", "Scene", "Texture2D", "SubTexture2D" };
			std::string old = AssetTypeToString(m_Options.Type).data();
			size_t pos = old.find_last_of(":");
			std::string newstr = old.substr(pos + 1, old.size());

			const char* currentAssetTypeString = newstr.c_str();

			UI::DrawCombo("Type", currentAssetTypeString, [&]() {
				for (int i = 0; i < (sizeof(AssetTypeString) / sizeof(const char*)); i++)
				{
					bool isSelected = currentAssetTypeString == AssetTypeString[i];
					if (ImGui::Selectable(AssetTypeString[i], isSelected))
					{
						currentAssetTypeString = AssetTypeString[i];
						std::string type = "AssetType::" + std::string(currentAssetTypeString);
						m_Options.Type = AssetTypeFromString(type);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				});

			if (ImGui::Button("Back"))
			{
				Close();
			}

			ImGui::SameLine();

			if (ImGui::Button("Confirm"))
			{
				EditorAssetManager editor;
				std::filesystem::path path = m_Options.FilePath;
				editor.ImportAsset(path);

				Close();
			}

			ImGui::End();
		}


		if (m_WasOpen && !m_IsOpen)
			Close();

	}
}
