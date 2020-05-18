#pragma once

#include "EditorWindow.hpp"
#include "Editor.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

#include "../Core/EntityName.hpp"

class HierarchyWindow : public EditorWindow
{
public:
	const char* GetName() { return "Hierarchy"; };

	HierarchyWindow(std::shared_ptr<entt::registry> registry, std::shared_ptr<Editor> editor) : registry(registry), editor(editor) {};

	void Update()
	{
		if (!isOpen)
			return;

		ImGuiIO io = ImGui::GetIO();

		ImGui::Begin(GetName(), &isOpen);

		std::vector<char*> entityNames;

		registry->view<EntityName>().each([&entityNames](auto entity, auto& entityName)
		{
			entityNames.push_back(entityName.name);
		});
		
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		float height = (vMax.y - vMin.y) - ImGui::GetStyle().WindowPadding.y * 0.5f;

		ImGui::PushItemWidth(-1);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0));
		ImGui::ListBox("", &editor->selectedEntity, &entityNames[0], entityNames.size(), (int) height / ImGui::GetTextLineHeightWithSpacing());
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::End();
	};
private:
	std::shared_ptr<entt::registry> registry;
	std::shared_ptr<Editor> editor;
};