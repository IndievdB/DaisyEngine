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

#include "Vendor/nlohmann/json.hpp"

void LoadScene(std::shared_ptr<entt::registry> registry)
{
	//json j;

	auto material = ResourceManager::GetInstance()->GetMaterial("GrayMat", "Resources/transform.shader");
	material->SetTexture("mainTex", "Resources/Prototype Gray.png");
	material = ResourceManager::GetInstance()->GetMaterial("BlueMat", "Resources/transform.shader");
	material->SetTexture("mainTex", "Resources/Prototype Blue.png");

	{
		auto entity = registry->create();
		Quaternion rotation = Quaternion(1, 0, 0, 0);
		registry->assign<EntityName>(entity, "Main Camera");
		registry->assign<Transform>(entity, Vector3(0.0f, 5.0f, -25.0f), Vector3::one, rotation);
		registry->assign<Camera>(entity);
		//registry->assign<LuaBehaviour>(entity, "Resources/FreeCam.lua");
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Ground Plane");
		registry->assign<Transform>(entity, Vector3::zero, Vector3::one, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "GrayMat", "Resources/plane.obj");
		PlaneCollider& collider = registry->assign<PlaneCollider>(entity);
		collider.normal = Vector3::up;
		collider.offset = 0;
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Cube");
		Vector3 position = Vector3(0, 5, 0);
		Vector3 scale = Vector3(2,1,1);
		Quaternion rotation = Quaternion(rand(), rand(), rand(), rand()); rotation.Normalize();
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BlueMat", "Resources/cube.obj");
		//registry->assign<LuaBehaviour>(entity, "Resources/FreeCam.lua");
	}
}