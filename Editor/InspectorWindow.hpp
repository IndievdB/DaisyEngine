#pragma once

#include "EditorWindow.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

class InspectorWindow : public EditorWindow
{
public:
	const char* GetName() { return "Inspector"; };

	InspectorWindow(std::shared_ptr<entt::registry> registry, std::shared_ptr<Editor> editor) : registry(registry), editor(editor) {};

	void Update()
	{
		if (!isOpen)
			return;

		ImGui::Begin(GetName(), &isOpen);

		auto view = registry->view<EntityName>();

		EntityName* entityName;
		int index = 0;
		int targetIndex = editor->selectedEntity;

		registry->view<EntityName>().each([&entityName, &index, &targetIndex](auto entity, auto& name)
		{
			if (index == targetIndex)
				entityName = &name;

			index++;
		});

		ImGui::Text("name");
		ImGui::SameLine();
		ImGui::InputText("##", entityName->name, IM_ARRAYSIZE(entityName->name));
		ImGui::End();
	};
private:
	std::shared_ptr<entt::registry> registry;
	std::shared_ptr<Editor> editor;
};