#pragma once

#include <time.h>
#include "Physics/RigidBody.hpp"
#include "Physics/BoxCollider.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/PlaneCollider.hpp"
#include "Renderer/MeshRenderer.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/TextMesh.hpp"
#include "Renderer/PointLight.hpp"
#include "Behaviour/LuaBehaviour.hpp"

#include <random>

float GetRandomFloat(const float min, const float max)
{
	std::random_device rd{};
	std::mt19937 engine{ rd() };
	std::uniform_real_distribution<float> dist{ min, max };
	return dist(engine);
}

void LoadScene(std::shared_ptr<entt::registry> registry)
{
	{
		auto material = ResourceManager::GetInstance()->GetMaterial("BrickMat", "Resources/Clustered/debug.shader");
		material->SetTexture("mainTex", "Resources/spnza_bricks_a_diff.tga");

		/*auto ironMat = ResourceManager::GetInstance()->GetMaterial("IronMat", "Resources/Clustered/debug.shader");
		ironMat->SetTexture("albedoMap", "Resources/PBR/iron/albedo.png");
		ironMat->SetTexture("normalMap", "Resources/PBR/iron/normal.png");
		ironMat->SetTexture("metallicMap", "Resources/PBR/iron/metallic.png");
		ironMat->SetTexture("roughnessMap", "Resources/PBR/iron/roughness.png");
		ironMat->SetTexture("aoMap", "Resources/PBR/iron/ao.png");*/
	}

	{
		auto entity = registry->create();
		Quaternion rotation = Quaternion(1, 0, 0, 0);
		registry->assign<Transform>(entity, Vector3(20,7.5,0), Vector3::one, rotation);
		registry->assign<Camera>(entity);
		registry->assign<LuaBehaviour>(entity, "Resources/FreeCam.lua");
	}
	
	{
		auto entity = registry->create();
		Vector3 position = Vector3(0,0,0);
		float diameter = 1.0f;
		Vector3 scale(0.01f, 0.01f, 0.01f);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BrickMat", "Resources/testCube.obj");
	}

	{
		const Vector3 minBounds = Vector3(-20, 1, -7.5f);
		const Vector3 maxBounds = Vector3(20, 10, 7.5f);
		const Vector3 lightsPerAxis = Vector3(8, 8, 8);

		Vector3 range = minBounds - maxBounds;
		const float xJump = fabs(range.x / lightsPerAxis.x);
		const float yJump = fabs(range.y / lightsPerAxis.y);
		const float zJump = fabs(range.z / lightsPerAxis.z);

		int lightCount = 0;

		for (int x = 0; x < lightsPerAxis.x; ++x)
		{
			for (int y = 0; y < lightsPerAxis.y; ++y)
			{
				for (int z = 0; z < lightsPerAxis.z; ++z)
				{
					float xCoord = minBounds.x + (x * xJump);
					float yCoord = minBounds.y + (y * yJump);
					float zCoord = minBounds.z + (z * zJump);

					float radius = 1.8f;
					float intensity = 2.75f;

					float r = GetRandomFloat(0, 1);
					float g = GetRandomFloat(0, 1);
					float b = GetRandomFloat(0, 1);

					Vector3 pos(xCoord, yCoord, zCoord);
					auto entity = registry->create();
					registry->assign<Transform>(entity, Vector3 (xCoord, yCoord, zCoord), Vector3::one, Quaternion::identity);
					registry->assign<PointLight>(entity, Vector4(r, g, b, 1), radius, intensity);
					++lightCount;
				}
			}
		}
	}
}