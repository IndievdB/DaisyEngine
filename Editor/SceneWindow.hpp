#pragma once

#include "EditorWindow.hpp"
#include "../Renderer/RenderSystem.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow(std::shared_ptr<RenderSystem> renderer) : renderer(renderer) {};
	const char* GetName() { return "Scene"; };

	void Update()
	{
		if (!isOpen)
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene", &isOpen);
		ImGui::PopStyleVar();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		ImGui::Image((void*)renderer->GetOffscreenTexture()->GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		Window::GetInstance()->SetViewportDimensions(size.x, size.y);
	};
protected:
	std::shared_ptr<RenderSystem> renderer;
};