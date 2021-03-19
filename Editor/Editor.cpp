#include "Editor.hpp"
#include "../Renderer/Window.hpp"
#include <GLFW\glfw3.h>

#include "SceneWindow.hpp"
#include "GameWindow.hpp"
#include "ProfilerWindow.hpp"
#include "ResourcesWindow.hpp"
#include "HierarchyWindow.hpp"
#include "InspectorWindow.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

Editor::Editor(std::shared_ptr<entt::registry> registry, std::shared_ptr<RenderSystem> renderer, std::shared_ptr<PhysicsSystem> physicsSystem, std::shared_ptr<LuaSystem> luaSystem) :
	registry(registry), renderer(renderer), physicsSystem(physicsSystem), luaSystem(luaSystem)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Window::GetInstance()->GetGLFWWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 150");
}

void Editor::AddWindows()
{
	windows.emplace_back(new ProfilerWindow);
	windows.emplace_back(new ResourcesWindow(shared_from_this()));
	windows.emplace_back(new InspectorWindow(shared_from_this()));
	windows.emplace_back(new HierarchyWindow(shared_from_this()));
	windows.emplace_back(new SceneWindow(shared_from_this()));
	windows.emplace_back(new GameWindow(shared_from_this()));
}

Editor::~Editor()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Editor::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	static bool show_app_dockspace = true;
	ImGui::Begin("DockSpace Demo", &show_app_dockspace, window_flags);
	ImGui::PopStyleVar(3);
	
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Entity"))
		{
			if (ImGui::MenuItem("Create"))
			{
				CreateEntity();
			}

			if (ImGui::BeginMenu("Add Component"))
			{
				if (ImGui::MenuItem("Transform"))
				{
					AddComponent<Transform>();
				}

				if (ImGui::MenuItem("Mesh Renderer"))
				{
					AddComponent<MeshRenderer>();
				}

				if (ImGui::MenuItem("Point Light"))
				{
					AddComponent<PointLight>();
				}

				if (ImGui::MenuItem("Directional Light"))
				{
					AddComponent<DirectionalLight>();
				}

				if (ImGui::MenuItem("Spot Light"))
				{
					AddComponent<SpotLight>();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			for (int i = 0; i < windows.size(); i++)
				ImGui::MenuItem(windows[i]->GetName(), "", &windows[i]->isOpen);

			ImGui::EndMenu();
		}
		//HelpMarker("help!");

		ImGui::EndMenuBar();
	}

	ImGui::End();

	for (int i = 0; i < windows.size(); i++)
		windows[i]->Update();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::FocusEntity(entt::entity& entity)
{
	selectedEntity = entity;

	for (auto& f : OnFocusEntity)
		f(entity);
}

void Editor::FocusResource(std::string resource)
{
	selectedEntity = entt::null;

	for (auto& f : OnFocusResource)
		f(resource);
}

void Editor::CreateEntity()
{
	auto entity = registry->create();
	registry->assign<EntityName>(entity, "New Entity");
	FocusEntity(entity);
}