#pragma once

#include "EditorWindow.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

#include <filesystem>

class ResourcesWindow : public EditorWindow
{
public:
	const char* GetName() { return "Asset Browser"; };

	ResourcesWindow()
	{
		for (const auto& entry : std::filesystem::directory_iterator("Resources"))
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

		ImGuiIO io = ImGui::GetIO();

		ImGui::Begin(GetName(), &isOpen);

		for (int i = 0; i < files.size(); i++)
		{
			DisplayNode(files[i]);
		}




		/*if (ImGui::TreeNode("folder"))
		{
			ImGui::Text("file");
			ImGui::Text("file");
			ImGui::Text("file");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("folder##"))
		{
			ImGui::Text("file");
			ImGui::Text("file");
			ImGui::Text("file");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("folder###"))
		{
			ImGui::Text("file");
			ImGui::Text("file");
			ImGui::Text("file");
			ImGui::TreePop();
		}*/

		ImGui::End();
	};
private:
	struct Node
	{
		std::filesystem::directory_entry entry;
		std::vector<Node> children;
		Node(std::filesystem::directory_entry entry) : entry(entry) {};
	};

	std::vector<Node> files;

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

	void FillNode(Node& rootNode)
	{
		if (!rootNode.entry.is_directory())
			return;

		for (const auto& entry : std::filesystem::directory_iterator(rootNode.entry))
		{
			Node subNode(entry);
			FillNode(subNode);
			files.push_back(subNode);
		}
	};
};