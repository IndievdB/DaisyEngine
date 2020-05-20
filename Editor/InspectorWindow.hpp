#pragma once

#include "EditorWindow.hpp"

#include "../Core/ResourceManager.hpp"
#include "../Core/Transform.hpp"
#include "../Renderer/MeshRenderer.hpp"
#include "../Renderer/Camera.hpp"
#include "../Renderer/Texture.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

#include <typeinfo>

class InspectorWindow : public EditorWindow
{
public:
	const char* GetName() { return "Inspector"; };

	InspectorWindow(std::shared_ptr<entt::registry> registry, std::shared_ptr<Editor> editor) : registry(registry), editor(editor) {};

	void Update()
	{
		if (!isOpen)
			return;

		int index = -1;
		int targetIndex = editor->selectedEntity;

		if (previouslySelectedEntity != targetIndex)
		{
			quaternionEulersCache.clear();
			quaternionsCache.clear();
			previouslySelectedEntity = targetIndex;
		}

		ImGui::Begin(GetName(), &isOpen);

		registry->view<EntityName>().each([this, &index, &targetIndex](auto& entity, auto& name)
		{
			index++;

			if (index != targetIndex)
				return;

			ImGui::Text("name");
			ImGui::SameLine();
			ImGui::InputText("##n", name.name, IM_ARRAYSIZE(name.name));
			ImGui::Separator();

			InputTransform(registry->try_get<Transform>(entity));
			InputCamera(registry->try_get<Camera>(entity));
			InputMeshRenderer(registry->try_get<MeshRenderer>(entity));
		});

		ImGui::End();
	};
private:
	std::shared_ptr<entt::registry> registry;
	std::shared_ptr<Editor> editor;
	std::unordered_map<std::string, Vector3> quaternionEulersCache;
	std::unordered_map<std::string, Quaternion> quaternionsCache;
	int previouslySelectedEntity;

	void InputVector3(std::string componentLabel, std::string label, Vector3& vector)
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		float width = vMax.x - vMin.x;

		ImGui::Text(label.c_str());
		ImGui::PushItemWidth(width / 3.25f);
		ImGui::DragFloat(("##" + componentLabel + label + "_x").c_str(), &(vector.x), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat(("##" + componentLabel + label + "_y").c_str(), &(vector.y), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat(("##" + componentLabel + label + "_z").c_str(), &(vector.z), 0.005f);
		ImGui::PopItemWidth();
	}

	void InputQuaternion(std::string componentLabel, std::string label, Quaternion& quaternion)
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		float width = vMax.x - vMin.x;

		std::string labelID = componentLabel + label;

		quaternionEulersCache.insert(std::pair<std::string, Vector3>(labelID, quaternion.GetEulerAngles()));
		quaternionsCache.insert(std::pair<std::string, Quaternion>(labelID, quaternion));

		if (quaternionsCache[labelID] != quaternion)
		{
			quaternionsCache[labelID] = quaternion;
			quaternionEulersCache[labelID] = quaternion.GetEulerAngles();
		}

		ImGui::Text(label.c_str());
		ImGui::PushItemWidth(width / 3.25f);
		ImGui::DragFloat(("##" + labelID + "_x").c_str(), &(quaternionEulersCache[labelID].x), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat(("##" + labelID + "_y").c_str(), &(quaternionEulersCache[labelID].y), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat(("##" + labelID + "_z").c_str(), &(quaternionEulersCache[labelID].z), 0.005f);
		ImGui::PopItemWidth();
		
		quaternion.Set(quaternionEulersCache[labelID].x, quaternionEulersCache[labelID].y, quaternionEulersCache[labelID].z);
		quaternionsCache[labelID] = quaternion;
	}

	void InputTransform(Transform* transform)
	{
		if (transform == nullptr)
			return;

		if (ImGui::CollapsingHeader("Transform"))
		{
			InputVector3("Transform", "Position", transform->position);
			InputVector3("Transform", "Scale", transform->scale);
			InputQuaternion("Transform", "Rotation", transform->rotation);
		}
	}

	void InputMesh(std::shared_ptr<Mesh>& mesh)
	{
		ImGui::Text("Mesh");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DragResource"))
			{
				IM_ASSERT(payload->DataSize == sizeof(std::string));
				std::string payload_n = *(const std::string*)payload->Data;
				mesh = ResourceManager::GetInstance()->GetMesh(payload_n);
			}
			ImGui::EndDragDropTarget();
		}
	}

	void InputMeshRenderer(MeshRenderer* meshRenderer)
	{
		if (meshRenderer == nullptr)
			return;

		if (ImGui::CollapsingHeader("Mesh Renderer"))
		{
			InputMesh(meshRenderer->mesh);

			ImGui::Text("Material");
		}
	}

	void InputCamera(Camera* camera)
	{
		if (camera == nullptr)
			return;

		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Text("FOV");
			ImGui::DragFloat("##Camera_FOV", &(camera->fov), 0.005f);
			ImGui::Text("Near Plane");
			ImGui::DragFloat("##Camera_NearPlane", &(camera->nearPlane), 0.005f);
			ImGui::Text("Far Plane");
			ImGui::DragFloat("##Camera_FarPlane", &(camera->farPlane), 0.005f);
		}
	}
};