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

		
		while (refresh_time < ImGui::GetTime())
		{
			ms = io.DeltaTime * 1000.0f;
			fps = (int)io.Framerate;
			values[values_offset] = ms;
			values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
			refresh_time += 1.0f / 15.0f;
		}

		ImGui::Begin(GetName(), &isOpen);
		ImGui::Text("%f ms (%i fps)\n", ms, fps);
		ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, "", -1.0f, 1.0f, ImVec2(0, 80));

		ImGui::End();
	};
private:
	float values[90];
	int values_offset = 0;
	double refresh_time = 0.0;
	float ms;
	int fps;

};