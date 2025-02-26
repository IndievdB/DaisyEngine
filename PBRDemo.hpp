#pragma once

#include <time.h>
#include "Physics/RigidBody.hpp"
#include "Physics/BoxCollider.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/PlaneCollider.hpp"
#include "Renderer/MeshRenderer.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/TextMesh.hpp"
#include "Behaviour/LuaBehaviour.hpp"
#include "Core/EntityName.hpp"

void LoadScene(std::shared_ptr<entt::registry> registry)
{
	{
		auto material = ResourceManager::GetInstance()->GetMaterial("BrickMat", "Resources/Clustered/debug.shader");
		material->SetTexture("mainTex", "Resources/spnza_bricks_a_diff.tga");

		auto ironMat = ResourceManager::GetInstance()->GetMaterial("IronMat", "Resources/Clustered/pbr.shader");
		ironMat->SetTexture("albedoMap", "Resources/PBR/iron/albedo.png");
		ironMat->SetTexture("normalMap", "Resources/PBR/iron/normal.png");
		ironMat->SetTexture("metallicMap", "Resources/PBR/iron/metallic.png");
		ironMat->SetTexture("roughnessMap", "Resources/PBR/iron/roughness.png");
		ironMat->SetTexture("aoMap", "Resources/PBR/iron/ao.png");

		auto goldMat = ResourceManager::GetInstance()->GetMaterial("GoldMat", "Resources/Clustered/pbr.shader");
		goldMat->SetTexture("albedoMap", "Resources/PBR/gold/albedo.png");
		goldMat->SetTexture("normalMap", "Resources/PBR/gold/normal.png");
		goldMat->SetTexture("metallicMap", "Resources/PBR/gold/metallic.png");
		goldMat->SetTexture("roughnessMap", "Resources/PBR/gold/roughness.png");
		goldMat->SetTexture("aoMap", "Resources/PBR/gold/ao.png");

		auto grassMat = ResourceManager::GetInstance()->GetMaterial("GrassMat", "Resources/Clustered/pbr.shader");
		grassMat->SetTexture("albedoMap", "Resources/PBR/grass/albedo.png");
		grassMat->SetTexture("normalMap", "Resources/PBR/grass/normal.png");
		grassMat->SetTexture("metallicMap", "Resources/PBR/grass/metallic.png");
		grassMat->SetTexture("roughnessMap", "Resources/PBR/grass/roughness.png");
		grassMat->SetTexture("aoMap", "Resources/PBR/grass/ao.png");

		auto plasticMat = ResourceManager::GetInstance()->GetMaterial("PlasticMat", "Resources/Clustered/pbr.shader");
		plasticMat->SetTexture("albedoMap", "Resources/PBR/plastic/albedo.png");
		plasticMat->SetTexture("normalMap", "Resources/PBR/plastic/normal.png");
		plasticMat->SetTexture("metallicMap", "Resources/PBR/plastic/metallic.png");
		plasticMat->SetTexture("roughnessMap", "Resources/PBR/plastic/roughness.png");
		plasticMat->SetTexture("aoMap", "Resources/PBR/plastic/ao.png");

		auto wallMat = ResourceManager::GetInstance()->GetMaterial("WallMat", "Resources/Clustered/pbr.shader");
		wallMat->SetTexture("albedoMap", "Resources/PBR/wall/albedo.png");
		wallMat->SetTexture("normalMap", "Resources/PBR/wall/normal.png");
		wallMat->SetTexture("metallicMap", "Resources/PBR/wall/metallic.png");
		wallMat->SetTexture("roughnessMap", "Resources/PBR/wall/roughness.png");
		wallMat->SetTexture("aoMap", "Resources/PBR/wall/ao.png");
	}
	
	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Camera");
		Quaternion rotation = Quaternion(1, 0, 0, 0);
		registry->assign<Transform>(entity, Vector3(0.0f, 0.0f, -10.0f), Vector3::one, rotation);
		registry->assign<Camera>(entity);
		registry->assign<LuaBehaviour>(entity, "Resources/FreeCam.lua");
	}
	
	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Iron Sphere");
		Vector3 position = Vector3(-4.0f,0,0);
		float diameter = 1.0f;
		Vector3 scale(diameter, diameter, diameter);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "IronMat", "Resources/sphere.obj");
	}
	
	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Gold Sphere");
		Vector3 position = Vector3(-2.0f, 0, 0);
		float diameter = 1.0f;
		Vector3 scale(diameter, diameter, diameter);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "GoldMat", "Resources/sphere.obj");
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Grass Sphere");
		Vector3 position = Vector3(0, 0, 0);
		float diameter = 1.0f;
		Vector3 scale(diameter, diameter, diameter);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "GrassMat", "Resources/sphere.obj");
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Plastic Sphere");
		Vector3 position = Vector3(2.0f, 0, 0);
		float diameter = 1.0f;
		Vector3 scale(diameter, diameter, diameter);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "PlasticMat", "Resources/sphere.obj");
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Brick Sphere");
		Vector3 position = Vector3(4.0f, 0, 0);
		float diameter = 1.0f;
		Vector3 scale(diameter, diameter, diameter);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "WallMat", "Resources/sphere.obj");
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Point Light");
		registry->assign<Transform>(entity, Vector3(0, 3, 0), Vector3::one, Quaternion::identity);
		registry->assign<PointLight>(entity, Vector4(1, 0, 0, 1), 100, 15, false);
	}
}