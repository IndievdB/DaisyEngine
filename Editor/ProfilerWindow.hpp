#pragma once

#include "EditorWindow.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

class ProfilerWindow : public EditorWindow
{
public:
	const char* GetName() { return "Profiler"; };

	void Update()
	{
		if (!isOpen)
			return;

		ImGuiIO io = ImGui::GetIO();

		ImGui::Begin("Profiling", &isOpen);
		ImGui::Text("%f ms\n", io.DeltaTime * 1000.0f);
		ImGui::Text("%f fps\n", io.Framerate);
		ImGui::End();
	};
};