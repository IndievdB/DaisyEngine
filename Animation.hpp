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

void LoadScene(std::shared_ptr<entt::registry> registry)
{
	auto material = ResourceManager::GetInstance()->GetMaterial("BlueMat", "Resources/transform.shader");
	material->SetTexture("mainTex", "Resources/Prototype Blue.png");
	material = ResourceManager::GetInstance()->GetMaterial("GrayMat", "Resources/transform.shader");
	material->SetTexture("mainTex", "Resources/Prototype Gray.png");

	{
		auto entity = registry->create();
		Quaternion rotation = Quaternion(1, 0, 0, 0);
		registry->assign<Transform>(entity, Vector3(0.0f, 5.0f, -25.0f), Vector3::one, rotation);
		registry->assign<Camera>(entity);
		registry->assign<LuaBehaviour>(entity, "Resources/FreeCam.lua");
	}

	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3::zero, Vector3::one, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "GrayMat", "Resources/plane.obj");
		PlaneCollider& collider = registry->assign<PlaneCollider>(entity);
		collider.normal = Vector3::up;
		collider.offset = 0;
	}

	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3::up * 5.0f, Vector3::one * 0.1f, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "BlueMat", "Resources/walking.fbx");
	}

}