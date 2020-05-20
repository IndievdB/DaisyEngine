#pragma once

#include "EditorWindow.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

#include <filesystem>

class ResourcesWindow : public EditorWindow
{
public:
	const char* GetName() { return "Resources"; };

	ResourcesWindow()
	{
		rootFolder = currentFolder = std::make_shared<std::filesystem::path>("Resources");
		folderImage = std::make_shared<Texture>("Resources/folder.png");
		fileImage = std::make_shared<Texture>("Resources/file.png");

		for (const auto& entry : std::filesystem::directory_iterator(*rootFolder))
		{
			Node rootNode(entry);
			FillNode(rootNode);
			files.push_back(rootNode);
		}
	}

	void Update()
	{
		if (!isOpen)
			return;

		ImGui::Begin(GetName(), &isOpen, ImGuiWindowFlags_MenuBar);

		std::vector <std::filesystem::path> parentPaths;
		parentPaths.push_back(*currentFolder);

		while (parentPaths.back() != *rootFolder)
			parentPaths.push_back(parentPaths.back().parent_path());

		ImGui::BeginMenuBar();

		for (int i = parentPaths.size() - 1; i >= 0; i--)
		{
			if (ImGui::Button(parentPaths[i].filename().string().c_str()))
			{
				currentFolder = std::make_shared<std::filesystem::path>(parentPaths[i]);
			}

			if (i != 0)
				ImGui::Text(">");
		}

		ImGui::EndMenuBar();
		
		/*static std::string draggedPath;
		static int selected = -1;
		int n = 0;

		for (auto& item : std::filesystem::directory_iterator(*currentFolder))
		{
			//ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, 0);
			ImGui::Image((void*)(item.is_directory() ? folderImage->GetTextureID() : fileImage->GetTextureID()), ImVec2(16,16), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();
			if (ImGui::Selectable(item.path().filename().string().c_str(), selected == n, ImGuiSelectableFlags_AllowDoubleClick))
			{
				selected = n;

				if (ImGui::IsMouseDoubleClicked(0))
				{
					if (item.is_directory())
					{
						currentFolder = std::make_shared<std::filesystem::path>(item.path());
					}
				}
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				draggedPath = item.path().string();
				ImGui::SetDragDropPayload("DragResource", &draggedPath, sizeof(std::string));    // Set payload to carry the index of our item (could be anything)
				ImGui::Text(item.path().filename().string().c_str());
				ImGui::EndDragDropSource();
			}

			n++;
		}*/

		for (int i = 0; i < files.size(); i++)
		{
			DisplayNode(files[i]);
		}
		
		ImGui::End();
	};
private:
	std::shared_ptr<Texture> fileImage;
	std::shared_ptr<Texture> folderImage;
	std::shared_ptr<std::filesystem::path> currentFolder;
	std::shared_ptr<std::filesystem::path> rootFolder;

	struct Node
	{
		std::filesystem::directory_entry entry;
		std::vector<Node> children;
		Node(std::filesystem::directory_entry entry) : entry(entry) {};
	};

	std::vector<Node> files;

	int NumberOfFilesInFilePath(std::shared_ptr<std::filesystem::path> path)
	{
		using std::filesystem::directory_iterator;
		return std::distance(directory_iterator(*path), directory_iterator{});
	};

	void FillNode(Node& rootNode)
	{
		if (!rootNode.entry.is_directory())
			return;

		for (const auto& entry : std::filesystem::directory_iterator(rootNode.entry))
		{
			Node subNode(entry);
			FillNode(subNode);
			rootNode.children.push_back(subNode);
		}
	};

	void DisplayNode(Node& rootNode)
	{
		if (!rootNode.entry.is_directory())
		{
			ImGui::Text(rootNode.entry.path().filename().string().c_str());
			return;
		}

		if (ImGui::TreeNode(rootNode.entry.path().filename().string().c_str()))
		{
			for (int i = 0; i < rootNode.children.size(); i++)
				DisplayNode(rootNode.children[i]);

			ImGui::TreePop();
		}

	};
};