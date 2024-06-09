#include "uypch.h"
#include "ContentBrowserPanel.h"

#include "UI/UI.h"
#include "Uneye/Project/Project.h"
#include "Uneye/Core/Filesystem.h"
#include "Uneye/Asset/TextureImporter.h"

#include "Uneye/Core/Timer.h"

#include "Uneye/Utils/StringUtils.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <stack>




namespace Uneye
{
	namespace fs = std::filesystem;

	namespace Utils
	{
		static void SearchFiles(const fs::path& p_directory, const std::string& p_query, std::vector<fs::path>& p_results)
		{
			for (const auto& entry : fs::recursive_directory_iterator(p_directory)) 
			{
				if (entry.is_regular_file() && entry.path().filename().string().find(p_query) != std::string::npos)
					p_results.push_back(entry.path());
			}
		}
	}

	ContentBrowserPanel::ContentBrowserPanel(Ref<Project> p_project)
		:m_Project(p_project), m_ThumbnailCache(CreateRef<ThumbnailCache>(p_project)), m_BaseDirectory(m_Project->GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		m_DirectoryIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIcon.png");
	}


	static std::stack<fs::path> s_FuturePaths;
	static void clearStack()
	{
		while (!s_FuturePaths.empty())
			s_FuturePaths.pop();
	}

	static std::vector<fs::path> s_SearchResults;
	static char s_SearchText[256] = "";

	void ContentBrowserPanel::OnImGuiRender()
	{

		ImGui::Begin("Content Browser");
		{
			ContentTopSearch();

			if (ImGui::BeginTable("##Main_Painel", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ContentTree();

				ImGui::TableSetColumnIndex(1);

				if (s_SearchResults.empty())
					ContentBrowser();
				else
					ContentSearchResult();

				ImGui::EndTable();
			}
		}
		ImGui::End();

		m_ThumbnailCache->OnUpdate();
	}

	void ContentBrowserPanel::ShowDirectoryTree(const fs::path& p_path)
	{
		if (fs::exists(p_path) && fs::is_directory(p_path))
		{
			for (const auto& entry : fs::directory_iterator(p_path))
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

	void ContentBrowserPanel::ContentTopSearch()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		// Top panel (Navigation bar)
		ImGui::BeginChild("top_panel", ImVec2(0, 30), false);
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
			ImGui::PushFont(FontManager::GetFont("Arrows"));
			if (ImGui::Button("b", { 0, 0 }))
			{
				if (m_CurrentDirectory != fs::path(m_BaseDirectory))
				{
					s_FuturePaths.push(m_CurrentDirectory);
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
				}

			}
			ImGui::PopFont();
			ImGui::PopStyleColor();


			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
			ImGui::PushFont(FontManager::GetFont("Arrows"));
			if (ImGui::Button("a", { 0, 0 }))
			{
				if (!s_FuturePaths.empty())
				{
					m_CurrentDirectory = s_FuturePaths.top();
					s_FuturePaths.pop();
				}
			}
			ImGui::PopFont();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
			ImGui::PushFont(FontManager::GetFont("Arrows"));
			if (ImGui::Button("Y"))
			{
				s_SearchText[0] = '\0';
				s_SearchResults.clear();
			}
			ImGui::PopFont();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			// Search input
			if (ImGui::InputTextWithHint("##Search", "Search...", s_SearchText, sizeof(s_SearchText)))
			{
				s_SearchResults.clear();
				if (strlen(s_SearchText) > 0)
				{
					Utils::SearchFiles(m_BaseDirectory, s_SearchText, s_SearchResults);
				}
			}

			ImGui::SameLine();

			std::vector<fs::path> pathSegments;
			fs::path pathAccumulator;

			auto currentRelPath = fs::relative(Project::GetActiveAssetFileSystemPath(m_CurrentDirectory), Project::GetActiveProjectDirectory());
			for (const auto& part : currentRelPath)
			{
				pathAccumulator /= part;
				pathSegments.push_back(pathAccumulator);
			}

			bool first = true;
			for (const auto& segment : pathSegments)
			{
				std::string filenameStr = segment.filename().string();

				if (!first)
				{
					ImGui::SameLine();
					ImGui::Text("/"); // Separator
					ImGui::SameLine();
				}

				ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.3f, 0.35f, 0.5f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.3f, 0.3f, 0.35f, 0.5f });
				if (ImGui::Button((filenameStr.empty()) ? "##Current" : filenameStr.c_str()))
				{
					if (m_CurrentDirectory.filename() != segment.filename())
					{
						clearStack();
						m_CurrentDirectory = Project::GetActiveProjectDirectory() / segment;
					}
				}
				ImGui::PopStyleColor(3);

				first = false;
			}

		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}



	void ContentBrowserPanel::ContentBrowser()
	{
		ImVec2 size = ImGui::GetContentRegionAvail();

		ImGui::BeginChild("right_panel", size);

		float cellSize = m_ThumbnailSize + m_Padding;

		float panelWidth = size.x;
		int columnCount = (int)std::abs(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		std::vector<fs::directory_entry> entries;
		for (auto& directoryEntry : fs::directory_iterator(m_CurrentDirectory))
			entries.push_back(directoryEntry);


		if (ImGui::BeginTable("##ContentBrowser", columnCount))
		{
			ImGuiListClipper clipper;
			int rowCount = (int)std::ceil((float)entries.size() / columnCount);
			clipper.Begin(rowCount, m_ThumbnailSize + ImGui::GetWindowSize().y * 0.33f);



			while (clipper.Step())
			{

				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
				{
					ImGui::TableNextRow();
					std::vector<std::pair<std::string, Ref<Texture2D>>> rowData;

					for (int column = 0; column < columnCount; column++)
					{
						int index = row * columnCount + column;
						if (index >= entries.size())
							break;

						const auto& directoryEntry = entries[index];
						const auto& path = directoryEntry.path();
						std::string filenameString = path.filename().string();

						Ref<Texture2D> thumbnail = m_DirectoryIcon;
						if (!fs::is_directory(path))
						{
							thumbnail = m_ThumbnailCache->GetOrCreateThumbnail(path);
							if (!thumbnail)
								thumbnail = m_FileIcon;
						}

						rowData.push_back({ filenameString, thumbnail });
					}

					for (int column = 0; column < columnCount && column < rowData.size(); column++)
					{

						ImGui::TableSetColumnIndex(column);
						int index = row * columnCount + column;

						const auto& [filenameString, thumbnail] = rowData[column];

						ImGui::PushID(filenameString.c_str());
						ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
						ImGui::ImageButton((ImTextureID)thumbnail->GetRendererID(), { m_ThumbnailSize, m_ThumbnailSize }, { 0, 1 }, { 1, 0 });

						if (!fs::is_directory(entries[index].path()) && ImGui::BeginDragDropSource())
						{
							auto relativePath = fs::relative(entries[index].path(), m_BaseDirectory);
							const wchar_t* itemPath = relativePath.c_str();
							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));

							ImGui::EndDragDropSource();
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::SetNextWindowSize({250.0f, 0});

							ImGui::BeginTooltip();

							std::string sizeStr = Utils::BytesToString(thumbnail->GetEstimatedSize());
							ImGui::TextWrapped("%s", filenameString.c_str());
							ImGui::Text("Mem: %s", sizeStr.c_str());
							ImGui::Text("Size: %i x %i", thumbnail->GetWidth(), thumbnail->GetHeight());

							ImGui::EndTooltip();
						}


						ImGui::PopStyleColor();

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							clearStack();
							if (fs::is_directory(entries[index].path()))
								m_CurrentDirectory /= entries[index].path().filename();
						}

						ImVec2 parent_size = ImGui::GetWindowSize();
						ImVec2 child_size(m_ThumbnailSize + 20.0f, parent_size.y * 0.2f);
						ImGui::BeginChild("##FILENAME_TEXT", child_size, false, ImGuiWindowFlags_NoScrollbar);

						ImGui::TextWrapped(filenameString.c_str());

						ImGui::EndChild();
						ImGui::PopID();

					}
				}
			}

			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

	void ContentBrowserPanel::ContentTree()
	{
		ImGui::BeginChild("left_panel", ImVec2(m_LeftPanelWidth, 0));
		{
			ShowDirectoryTree(m_BaseDirectory);
		}
		ImGui::EndChild();
	}

	void ContentBrowserPanel::ContentSearchResult()
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::BeginChild("search result", size);

		float cellSize = m_ThumbnailSize + m_Padding;

		float panelWidth = size.x;
		int columnCount = (int)std::abs(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;


		if (ImGui::BeginTable("##ContentBrowser", columnCount))
		{
			ImGuiListClipper clipper;
			int rowCount = (int)std::ceil((float)s_SearchResults.size() / columnCount);
			clipper.Begin(rowCount, m_ThumbnailSize + ImGui::GetWindowSize().y * 0.33f);

			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
				{
					ImGui::TableNextRow();
					std::vector<std::pair<std::string, Ref<Texture2D>>> rowData;

					for (int column = 0; column < columnCount; column++)
					{
						int index = row * columnCount + column;
						if (index >= s_SearchResults.size())
							break;

						const auto& path = s_SearchResults.at(index);
						std::string filenameString = path.filename().string();

						Ref<Texture2D> thumbnail = m_DirectoryIcon;
						if (!fs::is_directory(path))
						{
							thumbnail = m_ThumbnailCache->GetOrCreateThumbnail(path);
							if (!thumbnail)
								thumbnail = m_FileIcon;
						}

						rowData.push_back({ filenameString, thumbnail });
					}

					for (int column = 0; column < columnCount && column < rowData.size(); column++)
					{
						ImGui::TableSetColumnIndex(column);
						int index = row * columnCount + column;

						const auto& [filenameString, thumbnail] = rowData[column];

						ImGui::PushID(filenameString.c_str());
						ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
						ImGui::ImageButton((ImTextureID)thumbnail->GetRendererID(), { m_ThumbnailSize, m_ThumbnailSize }, { 0, 1 }, { 1, 0 });

						if (!fs::is_directory(s_SearchResults.at(index)) && ImGui::BeginDragDropSource())
						{
							auto relativePath = fs::relative(s_SearchResults.at(index), m_BaseDirectory);
							const wchar_t* itemPath = relativePath.c_str();
							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
							ImGui::EndDragDropSource();
						}

						ImGui::PopStyleColor();

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							clearStack();
							if (fs::is_directory(s_SearchResults.at(index)))
								m_CurrentDirectory /= s_SearchResults.at(index).filename();
						}

						ImVec2 parent_size = ImGui::GetWindowSize();
						ImVec2 child_size(m_ThumbnailSize + 20.0f, parent_size.y * 0.2f);
						ImGui::BeginChild("##FILENAME_TEXT", child_size, false, ImGuiWindowFlags_NoScrollbar);

						ImGui::TextWrapped(filenameString.c_str());

						ImGui::EndChild();
						ImGui::PopID();
					}
				}
			}

			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

}
