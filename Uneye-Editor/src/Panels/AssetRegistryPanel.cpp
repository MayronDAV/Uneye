#include "AssetRegistryPanel.h"
#include "Uneye/Asset/Asset.h"
#include "Uneye/Asset/AssetManager.h"

#include "Uneye/Utils/PlatformUtils.h"

#include "Uneye/Core/KeyCodes.h"

#include <imgui/imgui.h>



namespace Uneye
{
	void AssetRegistryPanel::OnImGuiRender()
	{
		ImGui::Begin("Asset Registry");

		if (ImGui::BeginTable("##AssetRegistryTable", 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 170.0f);
			ImGui::TableSetupColumn("Filename");
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();

			std::vector<AssetHandle> assetsRemoved;

			auto assetRegistry = AssetManager::GetAssetRegistry();
			auto it = assetRegistry.begin();

			ImGuiListClipper clipper;
			clipper.Begin(assetRegistry.size());
			static AssetHandle currentID;


			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
				{
					if (it == assetRegistry.end())
						break;

					auto asset = *it;

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					std::string id = std::to_string(asset.first);
					ImGui::Text(id.c_str());

					bool hovered_item = ImGui::IsItemHovered();

					ImGui::TableSetColumnIndex(1);
					
					std::string filenameStr = asset.second.FilePath.stem().string();
					ImGui::Text(filenameStr.c_str());

					hovered_item = hovered_item ||  ImGui::IsItemHovered();

					if (ImGui::IsItemHovered())
						currentID = asset.first;

					ImGui::PushID(currentID + asset.first);
					if (ImGui::BeginPopupContextWindow(0, 1))
					{
						if (ImGui::MenuItem("Import"))
						{
							std::string filepath = FileDialogs::OpenFile("All Files (*.*) | *.*");
							if (!filepath.empty())
								AssetManager::ImportAsset(filepath);
						}

						if (ImGui::MenuItem("Delete"))
							assetsRemoved.push_back(currentID);

						ImGui::EndPopup();
					}
					ImGui::PopID();

					if (hovered_item)
					{
						ImGui::BeginTooltip();

						std::string type = AssetTypeToString(asset.second.Type).data();
						size_t pos = type.find_last_of(":");
						std::string newType = type.substr(pos + 1, type.size());
						ImGui::Text("Type: %s", newType.c_str());

						std::string path = std::filesystem::relative(asset.second.FilePath, Project::GetActiveProjectDirectory()).string();
						ImGui::Text("Filepath: %s", path.c_str());

						ImGui::EndTooltip();
					}

					it++;
				}

				if (!assetsRemoved.empty())
				{
					for (const auto& id : assetsRemoved)
					{
						AssetManager::RemoveAsset(id);
					}

					assetsRemoved.clear();
				}
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}
}
