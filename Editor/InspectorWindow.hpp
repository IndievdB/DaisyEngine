#pragma once

#include "EditorWindow.hpp"

#include "../Core/ResourceManager.hpp"
#include "../Core/Transform.hpp"
#include "../Renderer/MeshRenderer.hpp"
#include "../Renderer/Camera.hpp"
#include "../Renderer/Texture.hpp"
#include "../Renderer/PointLight.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

#include "../Core/HelperFunctions.hpp"

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
			InputPointLight(editor->registry->try_get<PointLight>(selectedEntity));
			InputDirectionalLight(editor->registry->try_get<DirectionalLight>(selectedEntity));
			InputSpotLight(editor->registry->try_get<SpotLight>(selectedEntity));
		}

		ImGui::End();
	};
private:
	std::shared_ptr<Editor> editor;
	std::unordered_map<std::string, Vector3> quaternionEulersCache;
	std::unordered_map<std::string, Quaternion> quaternionCache;
	entt::entity selectedEntity;
	std::string selectedResource;

	void EntitySelected(entt::entity& entity)
	{
		if (selectedEntity != entity)
		{
			quaternionEulersCache.clear();
			quaternionCache.clear();
		}

		selectedResource = "";
		selectedEntity = entity;
	}

	void ResourceSelected(std::string resource)
	{
		quaternionEulersCache.clear();
		quaternionCache.clear();

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

	void InputVector3(std::string label, Vector3& vector)
	{
		ImGui::PushID(label.c_str());

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		float width = vMax.x - vMin.x;

		ImGui::Text(label.c_str());
		ImGui::PushItemWidth(width / 3.25f);
		ImGui::DragFloat("##x", &(vector.x), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat("##y", &(vector.y), 0.005f);
		ImGui::SameLine();
		ImGui::DragFloat("##z", &(vector.z), 0.005f);
		ImGui::PopItemWidth();

		ImGui::PopID();
	}

	void InputQuaternion(std::string quaternionID, std::string label, Quaternion& quaternion)
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		float width = vMax.x - vMin.x;

		// If no cache exists, setup cache
		if (quaternionEulersCache.find(quaternionID) == quaternionEulersCache.end())
		{
			quaternionEulersCache.insert(std::pair<std::string, Vector3>(quaternionID, quaternion.GetEulerAnglesZYX() * kRadToDeg));
			quaternionCache.insert(std::pair<std::string, Quaternion>(quaternionID, quaternion));
		}
		else
		{
			Quaternion cacheQuaternion = quaternionCache[quaternionID];

			// If the cache is out of date, update it
			if (quaternion != cacheQuaternion)
				quaternionEulersCache[quaternionID] = quaternion.GetEulerAnglesZYX() * kRadToDeg;
		}

		ImGui::Text(label.c_str());
		ImGui::PushItemWidth(width / 3.25f);
		bool xUpdated = ImGui::DragFloat(("##" + quaternionID + "_x").c_str(), &(quaternionEulersCache[quaternionID].x), 0.1f);
		ImGui::SameLine();
		bool yUpdated = ImGui::DragFloat(("##" + quaternionID + "_y").c_str(), &(quaternionEulersCache[quaternionID].y), 0.1f);
		ImGui::SameLine();
		bool zUpdated = ImGui::DragFloat(("##" + quaternionID + "_z").c_str(), &(quaternionEulersCache[quaternionID].z), 0.1f);
		ImGui::PopItemWidth();
		
		if (xUpdated || yUpdated || zUpdated)
		{
			Vector3 quaternionEuler = quaternionEulersCache[quaternionID] * kDegToRad;
			quaternion.SetZYX(quaternionEuler.x, quaternionEuler.y, quaternionEuler.z);
			quaternionCache[quaternionID] = quaternion;
		}
	}

	void InputTransform(Transform* transform)
	{
		if (transform == nullptr)
			return;

		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::PushID("Transform");
			InputVector3("Position", transform->position);
			InputVector3("Scale", transform->scale);
			InputQuaternion(std::to_string((uint32_t)selectedEntity) + "TransformRotation", "Rotation", transform->rotation);
			ImGui::PopID();
		}
	}

	void InputMesh(std::string meshID, std::string label, std::shared_ptr<Mesh>& mesh)
	{
		if (mesh.get() == nullptr)
			return;
		
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
				
				if (HelperFunctions::HasEnding(payload_n, ".obj") || HelperFunctions::HasEnding(payload_n, ".fbx"))
					mesh = ResourceManager::GetInstance()->GetMesh(payload_n);
			}
			ImGui::EndDragDropTarget();
		}
	}

	void InputMaterial(std::string materialID, std::string label, std::shared_ptr<Material>& material)
	{
		if (material.get() == nullptr)
			return;

		ImGui::Text(label.c_str());

		std::vector<std::string> materialURLs;
		for (const auto& entry : std::filesystem::recursive_directory_iterator("Resources"))
		{
			if (entry.path().extension() == ".material")
				materialURLs.push_back(entry.path().string());
		}

		std::string materialPath = ResourceManager::GetInstance()->ResourcePathFromMaterial(material).c_str();

		float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		ImGui::PushItemWidth(width);
		if (ImGui::BeginCombo(("##" + materialID).c_str(), materialPath.c_str(), ImGuiComboFlags_NoArrowButton))
		{
			for (int i = 0; i < materialURLs.size(); i++)
			{
				bool is_selected = (materialPath == materialURLs[i]);

				if (ImGui::Selectable(materialURLs[i].c_str(), is_selected))
				{
					material = ResourceManager::GetInstance()->GetMaterial(materialURLs[i]);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragResource"))
			{
				IM_ASSERT(payload->DataSize == sizeof(std::string));
				std::string payload_n = *(const std::string*)payload->Data;

				if (HelperFunctions::HasEnding(payload_n, ".material"))
					material = ResourceManager::GetInstance()->GetMaterial(payload_n);
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
			InputMesh(std::to_string((uint32_t)selectedEntity) + "MeshRendererMesh", "Mesh", meshRenderer->mesh);
			InputMaterial(std::to_string((uint32_t)selectedEntity) + "MeshRendererMaterial", "Material", meshRenderer->material);
		}
	}

	void InputCamera(Camera* camera)
	{
		if (camera == nullptr)
			return;

		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Text("FOV");
			ImGui::DragFloat("##Camera_FOV", &(camera->fov), 0.1f, 1.0f, 179.0f);
			ImGui::Text("Near Plane");
			ImGui::DragFloat("##Camera_NearPlane", &(camera->nearPlane), 0.005f);
			ImGui::Text("Far Plane");
			ImGui::DragFloat("##Camera_FarPlane", &(camera->farPlane), 0.005f);
		}
	}

	void InputPointLight(PointLight* pointLight)
	{
		if (pointLight == nullptr)
			return;

		if (ImGui::CollapsingHeader("Point Light"))
		{
			ImGui::PushID("Point Light");
			ImGui::Text("Color");
			ImGui::ColorEdit3("color", &(pointLight->color.x));
			ImGui::Text("Radius");
			ImGui::DragFloat("radius", &(pointLight->radius), 0.005f, 0.0f, 100.0f);
			ImGui::Text("Intensity");
			ImGui::DragFloat("intensity", &(pointLight->intensity), 0.005f, 0.0f, 100.0f);
			ImGui::PopID();
		}
	}

	void InputDirectionalLight(DirectionalLight* directionalLight)
	{
		if (directionalLight == nullptr)
			return;

		if (ImGui::CollapsingHeader("Directional Light"))
		{
			ImGui::PushID("Directional Light");
			ImGui::Text("Color");
			ImGui::ColorEdit3("color", &(directionalLight->color.x));
			//ImGui::Text("Direction");
			//ImGui::DragFloat("radius", &(pointLight->radius), 0.005f);
			ImGui::Text("Intensity");
			ImGui::DragFloat("intensity", &(directionalLight->intensity), 0.005f, 0.0f, 100.0f);
			ImGui::PopID();
		}
	}

	void InputSpotLight(SpotLight* spotLight)
	{
		if (spotLight == nullptr)
			return;

		if (ImGui::CollapsingHeader("Spot Light"))
		{
			ImGui::PushID("Spot Light");
			ImGui::Text("Color");
			ImGui::ColorEdit3("color", &(spotLight->color.x));
			ImGui::Text("Intensity");
			ImGui::DragFloat("intensity", &(spotLight->intensity), 0.005f, 0.0f, 100.0f);
			ImGui::Text("Cut Off");
			ImGui::DragFloat("cutoff", &(spotLight->cutOff), 0.1f, 0.01f, 90.0f);
			ImGui::PopID();
		}
	}
};