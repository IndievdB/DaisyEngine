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

		/*for (int i = 0; i < files.size(); i++)
		{
			DisplayNode(files[i]);
		}*/

		DragDropDemo();

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
			rootNode.children.push_back(subNode);
		}
	};

	void DragDropDemo()
	{
		if (ImGui::TreeNode("Drag and drop to copy/swap items"))
		{
			enum Mode
			{
				Mode_Copy,
				Mode_Move,
				Mode_Swap
			};
			static int mode = 0;
			if (ImGui::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } ImGui::SameLine();
			if (ImGui::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } ImGui::SameLine();
			if (ImGui::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }
			static const char* names[9] = { "Bobby", "Beatrice", "Betty", "Brianna", "Barry", "Bernard", "Bibi", "Blaine", "Bryn" };
			for (int n = 0; n < IM_ARRAYSIZE(names); n++)
			{
				ImGui::PushID(n);
				if ((n % 3) != 0)
					ImGui::SameLine();
				ImGui::Button(names[n], ImVec2(60, 60));

				// Our buttons are both drag sources and drag targets here!
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));    // Set payload to carry the index of our item (could be anything)
					if (mode == Mode_Copy) { ImGui::Text("Copy %s", names[n]); }    // Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
					if (mode == Mode_Move) { ImGui::Text("Move %s", names[n]); }
					if (mode == Mode_Swap) { ImGui::Text("Swap %s", names[n]); }
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int payload_n = *(const int*)payload->Data;
						if (mode == Mode_Copy)
						{
							names[n] = names[payload_n];
						}
						if (mode == Mode_Move)
						{
							names[n] = names[payload_n];
							names[payload_n] = "";
						}
						if (mode == Mode_Swap)
						{
							const char* tmp = names[n];
							names[n] = names[payload_n];
							names[payload_n] = tmp;
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	};
};