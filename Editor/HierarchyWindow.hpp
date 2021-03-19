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

	HierarchyWindow(std::shared_ptr<Editor> editor) : editor(editor)
	{
		editor->OnFocusEntity.push_back([=](entt::entity& e) { this->EntitySelected(e); });
		editor->OnFocusResource.push_back([=](std::string r) { this->ResourceSelected(r); });
	};

	void Update()
	{
		if (!isOpen)
			return;

		ImGuiIO io = ImGui::GetIO();

		ImGui::Begin(GetName(), &isOpen);

		//static int selected = -1;
		int n = 0;

		editor->registry->view<EntityName>().each([this, &n](auto entity, auto& entityName)
		{
			ImGui::PushID(n);

			if (ImGui::Selectable(entityName.name, selectedIndex == n, ImGuiSelectableFlags_AllowDoubleClick))
			{
				editor->FocusEntity(entity);
				selectedIndex = n;
			}

			ImGui::PopID();

			n++;
		});

		ImGui::End();
	};
private:
	std::shared_ptr<Editor> editor;
	int selectedIndex = -1;

	void EntitySelected(entt::entity& entity)
	{
		int n = 0;
		editor->registry->view<EntityName>().each([this, entity, &n](entt::entity e, auto& entityName)
		{
			if (entity == e)
				selectedIndex = n;

			n++;
		});
	}

	void ResourceSelected(std::string resource)
	{
		selectedIndex = -1;
	}
};