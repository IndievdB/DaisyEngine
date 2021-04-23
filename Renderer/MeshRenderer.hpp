#pragma once

#include "../Core/ResourceManager.hpp"

struct MeshRenderer
{
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
	bool castsShadows;

	MeshRenderer(std::string materialPath, std::string meshPath, bool castsShadows)
	{
		material = ResourceManager::GetInstance()->GetMaterial(materialPath);
		mesh = ResourceManager::GetInstance()->GetMesh(meshPath);
		this->castsShadows = castsShadows;
	};

	MeshRenderer(std::string materialPath, std::string meshPath)
	{
		material = ResourceManager::GetInstance()->GetMaterial(materialPath);
		mesh = ResourceManager::GetInstance()->GetMesh(meshPath);
		castsShadows = true;
	};

	MeshRenderer()
	{
		mesh = ResourceManager::GetInstance()->GetMesh("Resources/Engine/Meshes/DefaultCube.obj");
		material = ResourceManager::GetInstance()->GetMaterial("Resources/Engine/Materials/DefaultGrayGrid.material");
		this->castsShadows = true;
	};
};