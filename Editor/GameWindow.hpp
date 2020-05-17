#pragma once

#include "EditorWindow.hpp"
#include "../Renderer/RenderSystem.hpp"
#include "../Renderer/Window.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

class GameWindow : public EditorWindow
{
public:
	GameWindow(std::shared_ptr<RenderSystem> renderer, std::shared_ptr<entt::registry> registry, std::shared_ptr<PhysicsSystem> physicsSystem, std::shared_ptr<LuaSystem> luaSystem)
		: renderer(renderer), registry(registry), physicsSystem(physicsSystem), luaSystem(luaSystem)
	{
		renderTexture = std::make_shared<Texture>(Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight(), GL_RGB32F, GL_RGB);
		depthRenderbuffer = std::make_shared<Renderbuffer>(GL_DEPTH_COMPONENT24, Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight());
	};

	const char* GetName() { return "Game"; };

	void Update()
	{
		if (!isOpen)
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(GetName(), &isOpen, ImGuiWindowFlags_MenuBar);
		ImGui::PopStyleVar();
		
		ImGui::BeginMenuBar();
		ImVec4 selectedColor(0.2176f, 0.8058f, 0.8333f, 1.0f);
		int styleStack = 0;

		// PLAY

		if (isPlaying)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, selectedColor);
			styleStack++;
		}

		if (ImGui::Button("Play"))
		{
			isPlaying = true;
		}

		if (styleStack > 0)
		{
			ImGui::PopStyleColor();
			styleStack--;
		}

		// STOP

		if (!isPlaying)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, selectedColor);
			styleStack++;
		}

		if (ImGui::Button("Stop"))
		{
			isPlaying = false;
		}

		if (styleStack > 0)
		{
			ImGui::PopStyleColor();
			styleStack--;
		}

		ImGui::EndMenuBar();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		if (isPlaying)
		{
			physicsSystem->RunPhysics(registry);
		}



		std::shared_ptr<Transform> cameraTransform;
		std::shared_ptr<Camera> camera;

		registry->view<Transform, Camera>().each([&cameraTransform, &camera](auto& transform, auto& cam)
		{
				cameraTransform = std::make_shared<Transform>(transform);
				camera = std::make_shared<Camera>(cam);
		});

		if (camera == nullptr)
		{
			ImGui::End();
			return;
		}

		Window::GetInstance()->SetViewportDimensions(size.x, size.y);
		renderTexture->Reformat(Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight(), GL_RGB32F, GL_RGB);
		depthRenderbuffer->Reformat(GL_DEPTH_COMPONENT24, Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight());
		renderer->RenderAll(renderTexture, depthRenderbuffer, cameraTransform, camera);
		ImGui::Image((void*)renderTexture->GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	};
private:
	bool isPlaying = false;
	std::shared_ptr<Texture> renderTexture;
	std::shared_ptr<Renderbuffer> depthRenderbuffer;
	std::shared_ptr<entt::registry> registry;
	std::shared_ptr<RenderSystem> renderer;
	std::shared_ptr<PhysicsSystem> physicsSystem;
	std::shared_ptr<LuaSystem> luaSystem;
};