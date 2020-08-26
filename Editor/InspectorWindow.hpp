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

#include <filesystem>
#include <typeinfo>

class InspectorWindow : public EditorWindow
{
public:
	const char* GetName() { return "Inspector"; };

	InspectorWindow(std::shared_ptr<Editor> editor) : editor(editor)
	{
		editor->OnFocusEntity.push_back([=](entt::entity& e) { this->EntitySelected(e); });
		editor->OnFocusResource.push_back([=](std::string r) { this->ResourceSelected(r); });
		selectedEntity = entt::null;
	};

	void Update()
	{
		if (!isOpen)
			return;

		ImGui::Begin(GetName(), &isOpen);

		if (selectedEntity != entt::null && editor->registry->valid(selectedEntity))
		{
			InputEntityName(editor->registry->try_get<EntityName>(selectedEntity));
			InputTransform(editor->registry->try_get<Transform>(selectedEntity));
			InputCamera(editor->registry->try_get<Camera>(selectedEntity));
			InputMeshRenderer(editor->registry->try_get<MeshRenderer>(selectedEntity));
		}

		ImGui::End();
	};
private:
	std::shared_ptr<Editor> editor;
	std::unordered_map<std::string, Vector3> quaternionEulersCache;
	std::unordered_map<std::string, Quaternion> quaternionsCache;
	//std::unordered_map<std::string, std::string> MeshIDCache;
	entt::entity selectedEntity;
	std::string selectedResource;

	void EntitySelected(entt::entity& entity)
	{
		selectedResource = "";
		selectedEntity = entity;
	}

	void ResourceSelected(std::string resource)
	{
		selectedEntity = entt::null;
		selectedResource = resource;
	}

	void InputEntityName(EntityName* entityName)
	{
		if (entityName == nullptr)
			return;

		ImGui::Text("name");
		ImGui::SameLine();
		ImGui::InputText("##n", entityName->name, IM_ARRAYSIZE(entityName->name));
		ImGui::Separator();
	}

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

	void InputQuaternion(std::string quaternionID, std::string label, Quaternion& quaternion)
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		float width = vMax.x - vMin.x;

		quaternionEulersCache.insert(std::pair<std::string, Vector3>(quaternionID, quaternion.GetEulerAngles()));
		quaternionsCache.insert(std::pair<std::string, Quaternion>(quaternionID, quaternion));

		if (quaternionsCache[quaternionID] != quaternion)
		{
			quaternionsCache[quaternionID] = quaternion;
			quaternionEulersCache[quaternionID] = quaternion.GetEulerAngles();
		}

		ImGui::Text(label.c_str());
		ImGui::PushItemWidth(width / 3.25f);
		ImGui::DragFloat(("##" + quaternionID + "_x").c_str(), &(quaternionEulersCache[quaternionID].x), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat(("##" + quaternionID + "_y").c_str(), &(quaternionEulersCache[quaternionID].y), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat(("##" + quaternionID + "_z").c_str(), &(quaternionEulersCache[quaternionID].z), 0.005f);
		ImGui::PopItemWidth();
		
		quaternion.Set(quaternionEulersCache[quaternionID].x, quaternionEulersCache[quaternionID].y, quaternionEulersCache[quaternionID].z);
		quaternionsCache[quaternionID] = quaternion;
	}

	void InputTransform(Transform* transform)
	{
		if (transform == nullptr)
			return;

		if (ImGui::CollapsingHeader("Transform"))
		{
			InputVector3("Transform", "Position", transform->position);
			InputVector3("Transform", "Scale", transform->scale);
			InputQuaternion(std::to_string((uint32_t)selectedEntity) + "TransformRotation", "Rotation", transform->rotation);
		}
	}

	void InputMesh(std::string meshID, std::string label, std::shared_ptr<Mesh>& mesh)
	{
		ImGui::Text(label.c_str());
		
		std::vector<std::string> meshURLs;
		for (const auto& entry : std::filesystem::recursive_directory_iterator("Resources"))
		{
			if (entry.path().extension() == ".fbx" || entry.path().extension() == ".obj")
				meshURLs.push_back(entry.path().string());
		}

		std::string meshPath = ResourceManager::GetInstance()->ResourcePathFromMesh(mesh).c_str();
		
		float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		ImGui::PushItemWidth(width);
		if (ImGui::BeginCombo(("##" + meshID).c_str(), meshPath.c_str(), ImGuiComboFlags_NoArrowButton))
		{
			for (int i = 0; i < meshURLs.size(); i++)
			{
				bool is_selected = (meshPath == meshURLs[i]);

				if (ImGui::Selectable(meshURLs[i].c_str(), is_selected))
				{
					//item_current = meshURLs[i];
					mesh = ResourceManager::GetInstance()->GetMesh(meshURLs[i]);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DragResource"))
			{
				IM_ASSERT(payload->DataSize == sizeof(std::string));
				std::string payload_n = *(const std::string*)payload->Data;
				mesh = ResourceManager::GetInstance()->GetMesh(payload_n);

				//meshPath = payload_n;
			}
			ImGui::EndDragDropTarget();
		}
	}

	void InputMaterial(std::shared_ptr<Material>& material)
	{
		ImGui::Text("Material");
	}

	void InputMeshRenderer(MeshRenderer* meshRenderer)
	{
		if (meshRenderer == nullptr)
			return;

		if (ImGui::CollapsingHeader("Mesh Renderer"))
		{
			InputMesh(std::to_string((uint32_t)selectedEntity) + "MeshRendererMesh", "Mesh", meshRenderer->mesh);
			InputMaterial(meshRenderer->material);
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