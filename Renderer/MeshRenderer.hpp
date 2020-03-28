#pragma once

#include "../Core/ResourceManager.hpp"

struct MeshRenderer
{
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;

	MeshRenderer(std::string materialName, std::string meshPath)
	{
		material = ResourceManager::GetInstance()->GetMaterial(materialName);
		mesh = ResourceManager::GetInstance()->GetMesh(meshPath);
	};
};