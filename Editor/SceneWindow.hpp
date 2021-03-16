#pragma once

#include "EditorWindow.hpp"
#include "../Renderer/RenderSystem.hpp"
#include "../Renderer/Window.hpp"

#include "../Renderer/Camera.hpp"
#include "../Core/Transform.hpp"
#include "../Core/Input.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow(std::shared_ptr<Editor> editor) : editor(editor)
	{
		camera = std::make_shared<Camera>();
		cameraTransform = std::make_shared<Transform>(Vector3(0.0f, 5.0f, -25.0f), Vector3::one, Quaternion::identity);
		renderTexture = std::make_shared<Texture>(Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight(), GL_RGB32F, GL_RGB);
		depthRenderbuffer = std::make_shared<Renderbuffer>(GL_DEPTH_COMPONENT24, Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight());
	};

	const char* GetName() { return "Scene"; };

	void Update()
	{
		if (!isOpen)
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(GetName(), &isOpen);
		ImGui::PopStyleVar();

		ImGuiIO io = ImGui::GetIO();

		if (ImGui::IsWindowHovered() && io.MouseClicked[ImGuiMouseButton_Right])
		{
			Window::GetInstance()->SetCursorVisible(false);
			isDraggingMouse = true;
		}

		if (!io.MouseDown[ImGuiMouseButton_Right])
		{
			Window::GetInstance()->SetCursorVisible(true);
			isDraggingMouse = false;
		}

		if (isDraggingMouse)
		{
			if (io.KeysDown[KeyCode::A])
				cameraTransform->position += cameraTransform->GetRight() * 0.2f;

			if (io.KeysDown[KeyCode::D])
				cameraTransform->position -= cameraTransform->GetRight() * 0.2f;

			if (io.KeysDown[KeyCode::E])
				cameraTransform->position += cameraTransform->GetUp() * 0.2f;

			if (io.KeysDown[KeyCode::Q])
				cameraTransform->position -= cameraTransform->GetUp() * 0.2f;

			if (io.KeysDown[KeyCode::W])
				cameraTransform->position += cameraTransform->GetForward() * 0.2f;

			if (io.KeysDown[KeyCode::S])
				cameraTransform->position -= cameraTransform->GetForward() * 0.2f;

			cameraPitch += io.MouseDelta.y * 0.002f;
			cameraYaw -= io.MouseDelta.x * 0.002f;
			cameraTransform->rotation.SetZYX(cameraPitch, cameraYaw, 0);
		}


		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		Window::GetInstance()->SetViewportDimensions(size.x, size.y);
		renderTexture->Reformat(Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight(), GL_RGB32F, GL_RGB);
		depthRenderbuffer->Reformat(GL_DEPTH_COMPONENT24, Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight());
		editor->renderer->RenderAll(renderTexture, depthRenderbuffer, cameraTransform, camera);
		ImGui::Image((void*)renderTexture->GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	};
private:
	std::shared_ptr<Texture> renderTexture;
	std::shared_ptr<Renderbuffer> depthRenderbuffer;
	std::shared_ptr<Editor> editor;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Transform> cameraTransform;
	float cameraPitch = 0;
	float cameraYaw = 0;
	bool isDraggingMouse = false;
};