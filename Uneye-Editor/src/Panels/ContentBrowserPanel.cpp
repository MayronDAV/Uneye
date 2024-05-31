#include "uypch.h"
#include "ContentBrowserPanel.h"

#include "UI/UI.h"
#include "Uneye/Project/Project.h"

#include "Uneye/Core/Filesystem.h"

#include <imgui/imgui.h>
#include <stack>
#include <imgui/imgui_internal.h>
#include <regex>



namespace Uneye
{
	namespace fs = std::filesystem;

	namespace Utils
	{
		// TODO: Remove this when creating an asset manager

		static std::unordered_map<std::string, Ref<Texture2D>> textures;

		static Ref<Texture2D> CreateTexture(const std::string& path)
		{
			auto it = textures.find(path);
			if (it == textures.end())
				textures[path] = Texture2D::Create(path);

			return textures[path];
		}
		
		static void SearchFiles(const fs::path& directory, const std::string& query, std::vector<fs::path>& results) {
			for (const auto& entry : fs::recursive_directory_iterator(directory)) {
				if (entry.is_regular_file()) {
					if (entry.path().filename().string().find(query) != std::string::npos) {
						results.push_back(entry.path());
					}
				}
			}
		}
	}

	ContentBrowserPanel::ContentBrowserPanel()
		:m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::ShowDirectoryTree(const fs::path& path)
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


	static std::stack<fs::path> s_FuturePaths;
	static void clearStack()
	{
		while (!s_FuturePaths.empty())
			s_FuturePaths.pop();
	}

	static std::vector<fs::path> searchResults;
	static char searchText[256] = "";

	void ContentBrowserPanel::OnImGuiRender()
	{

		static float splitterWidth = 8.0f;
		static float leftPanelWidth = 150.0f;

		ImGui::Begin("Content Browser");
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			// Top panel (Navigation bar)
			ImGui::BeginChild("top_panel", ImVec2(0, 30), false);
			{
				ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
				ImGui::PushFont(FontManager::GetFont("Arrows"));
				if (ImGui::Button("b", {0, 0}))
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
				if (ImGui::Button("a", {0, 0}))
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
					searchText[0] = '\0';
					searchResults.clear();
				}
				ImGui::PopFont();
				ImGui::PopStyleColor();

				ImGui::SameLine();

				// Search input
				if (ImGui::InputTextWithHint("##Search", "Search...", searchText, sizeof(searchText)))
				{
					searchResults.clear();
					if (strlen(searchText) > 0)
					{
						Utils::SearchFiles(m_BaseDirectory, searchText, searchResults);
					}
				}

				ImGui::SameLine();

				std::vector<fs::path> pathSegments;
				fs::path pathAccumulator;

				auto currentRelPath = Project::GetAssetFileSystemPath(fs::relative(m_CurrentDirectory, m_BaseDirectory));
				for (const auto& part : currentRelPath)
				{
					if (part != ".")
					{
						pathAccumulator /= part;
						pathSegments.push_back(pathAccumulator);
					}
				}

				bool first = true;
				for (const auto& segment : pathSegments)
				{
					if (!first)
					{
						ImGui::SameLine();
						ImGui::Text("/"); // Separator
						ImGui::SameLine();
					}

					auto segmentSTR = fs::relative(segment, m_BaseDirectory);
					auto assetRelPath = Project::GetAssetFileSystemPath(segmentSTR);
					auto filenameStr = assetRelPath.filename().string();
					//UNEYE_CORE_TRACE("path: {}", filenameStr);
					if (filenameStr == "..")
						continue;
					else if (filenameStr == ".")
						filenameStr = Project::GetAssetDirectory().filename().string();

					ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.3f, 0.35f, 0.5f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.3f, 0.3f, 0.35f, 0.5f });
					if (ImGui::Button((filenameStr.empty()) ? "##Current" : filenameStr.c_str()))
					{
						clearStack();
						m_CurrentDirectory = segment;
					}
					ImGui::PopStyleColor(3);

					first = false;
				}

			}
			ImGui::EndChild();

			//ImGui::Separator();

			ImGui::BeginChild("main_panel", ImVec2(0, 0), false);
			{
				// Left panel (Directory Tree)
				ImGui::BeginChild("left_panel", ImVec2(leftPanelWidth, 0), true);
				{
					ShowDirectoryTree(m_BaseDirectory);
				}
				ImGui::EndChild();

				ImGui::SameLine();

				// Resize handle
				ImGui::Button("##Splitter", ImVec2(splitterWidth, -1));
				if (ImGui::IsItemHovered())
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				}
				if (ImGui::IsItemActive())
				{
					leftPanelWidth += ImGui::GetIO().MouseDelta.x;
					if (leftPanelWidth < 100.0f) leftPanelWidth = 100.0f;
				}
				ImGui::SameLine();

				ImVec2 size = ImGui::GetContentRegionAvail();
				if (searchResults.empty())
				{
					// Right panel (Content Browser)
					ImGui::BeginChild("right_panel", size, true);

					static float padding = 10.0f;
					static float thumbnailSize = 64.0f;
					float cellSize = thumbnailSize + padding;

					float panelWidth = ImGui::GetContentRegionAvail().x;
					int columnCount = (int)(panelWidth / cellSize);
					if (columnCount < 1)
						columnCount = 1;

					ImGui::Columns(columnCount, 0, false);

					for (auto& directoryEntry : fs::directory_iterator(m_CurrentDirectory))
					{
						const auto& path = directoryEntry.path();
						std::string filenameString = path.filename().string();
						
						Ref<Texture2D> icon = nullptr;
						if (directoryEntry.is_directory())
							icon = m_DirectoryIcon;

						if (directoryEntry.is_regular_file())
						{
							if (path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".jpeg")
							{
								icon = Utils::CreateTexture(path.string());
							}
							else
								icon = m_FileIcon;
						}

						ImGui::PushID(filenameString.c_str());
						ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
						ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

						if (ImGui::BeginDragDropSource())
						{
							auto relativePath = fs::relative(path, m_BaseDirectory);
							const wchar_t* itemPath = relativePath.c_str();
							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
							ImGui::EndDragDropSource();
						}
						
						ImGui::PopStyleColor();

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							clearStack();
							if (directoryEntry.is_directory())
								m_CurrentDirectory /= path.filename();
						}
						ImGui::TextWrapped(filenameString.c_str());

						ImGui::NextColumn();
						ImGui::PopID();
					}

					ImGui::EndChild();
				}
				else
				{
					ImGui::BeginChild("Seacrh Results", size, true);
					
					// Display search results
					for (const auto& result : searchResults)
					{

						if (ImGui::Selectable(result.string().c_str()))
						{
							if (fs::is_directory(result))
							{
								clearStack();
								m_CurrentDirectory = result;
								searchResults.clear();
							}
							else
							{
								// Handle file opening logic here
							}
						}

						if (ImGui::BeginDragDropSource())
						{
							auto relativePath = fs::relative(result, m_BaseDirectory);
							const wchar_t* itemPath = relativePath.c_str();
							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
							ImGui::EndDragDropSource();
						}
					}

					ImGui::EndChild();
				}

				ImGui::PopStyleVar();
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}
}
