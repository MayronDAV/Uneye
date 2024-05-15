#include "uypch.h"
#include "ContentBrowserPanel.h"

#include "UI/UI.h"

#include <imgui/imgui.h>



namespace Uneye
{
	namespace fs = std::filesystem;

	// TODO: change this for project directory
	static const fs::path s_AssetPath = "assets";


	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_AssetPath)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	static void ShowDirectoryTree(const fs::path& path)
	{
		if (fs::exists(path) && fs::is_directory(path))
		{
			for (const auto& entry : fs::directory_iterator(path)) 
			{
				const auto& entryPath = entry.path();

				if (fs::is_directory(entry.status())) 
				{

					if (ImGui::TreeNode(entryPath.filename().string().c_str()))
					{
						ShowDirectoryTree(entryPath);

						ImGui::TreePop();
					}
				}
				else
				{

					ImGui::Text("%s", entryPath.filename().string().c_str());
				}
			}
		}
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		//ImGui::Begin("Content Browser");

		//ShowDirectoryTree("assets");

		//ImGui::End();

		ImGui::Begin("File Browser");

		if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
		{
			ImGui::PushFont(FontManager::GetFont("Arrows"));
			if (ImGui::Button("B"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
			ImGui::PopFont();
		}

		static float padding = 10.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			std::string filenameString = relativePath.filename().string();

			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.3f, 0.35f, 1 });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.3f, 0.3f, 0.35f, 1.0f });
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::PopStyleColor(3);
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

		}

		// TODO: status bar
		ImGui::End();
	}
}
