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

	HierarchyWindow(std::shared_ptr<Editor> editor) : editor(editor) {};

	void Update()
	{
		if (!isOpen)
			return;

		ImGuiIO io = ImGui::GetIO();

		ImGui::Begin(GetName(), &isOpen);

		static int selected = -1;
		int n = 0;

		editor->registry->view<EntityName>().each([this, &n](auto entity, auto& entityName)
		{
			if (ImGui::Selectable(entityName.name, selected == n, ImGuiSelectableFlags_AllowDoubleClick))
			{
				editor->FocusEntity(entity);
				selected = n;
			}

			n++;
		});

		ImGui::End();
	};
private:
	std::shared_ptr<Editor> editor;
};