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
#include "Renderer/DirectionalLight.hpp"

void LoadScene(std::shared_ptr<entt::registry> registry)
{
	auto material = ResourceManager::GetInstance()->GetMaterial("BrickMat", "Resources/Clustered/debug.shader");
	material->SetTexture("mainTex", "Resources/spnza_bricks_a_diff.tga");

	{
		auto entity = registry->create();
		Quaternion rotation = Quaternion(1, 0, 0, 0);
		registry->assign<Transform>(entity, Vector3(0.0f, 5.0f, -5.0f), Vector3::one, rotation);
		registry->assign<Camera>(entity);
		registry->assign<LuaBehaviour>(entity, "Resources/FreeCam.lua");
	}

	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3::zero, Vector3::one, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "BrickMat", "Resources/plane.obj");
		PlaneCollider& collider = registry->assign<PlaneCollider>(entity);
		collider.normal = Vector3::up;
		collider.offset = 0;
	}

	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			auto entity = registry->create();
			Vector3 position = Vector3(-9 + i * 2.0f, 5, -j * 8 + 26.0f);
			Vector3 scale(1, 1, 1);
			Quaternion rotation = Quaternion::identity;
			Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
			registry->assign<MeshRenderer>(entity, "BrickMat", "Resources/sphere.obj");
		}

		for (int i = 0; i < 10; i++)
		{
			auto entity = registry->create();
			Vector3 position = Vector3(-9 + i * 2.0f, 5, -j * 8 + 30.0f);
			Vector3 scale = Vector3(1, 1, 2);
			Quaternion rotation = Quaternion::identity;
			Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
			registry->assign<MeshRenderer>(entity, "BrickMat", "Resources/cube.obj");
		}
	}
	

	/*
	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3(0, 8, 1), Vector3::one, Quaternion::identity);
		registry->assign<PointLight>(entity, Vector4(1, 0, 0, 1), 10.0f, 2.0f);
	}
	{
		auto entity = registry->create();
		Vector3 position = Vector3(0, 8, 0);
		Vector3 scale(0.2f, 0.2f, 0.2f);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BrickMat", "Resources/sphere.obj");
	}
	
	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3(0, 8, -20), Vector3::one, Quaternion::identity);
		registry->assign<PointLight>(entity, Vector4(1, 0, 0, 1), 10.0f, 2.0f);
	}
	{
		auto entity = registry->create();
		Vector3 position = Vector3(0, 8, -20);
		Vector3 scale(0.2f, 0.2f, 0.2f);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BrickMat", "Resources/sphere.obj");
	}
	*/



	//for (int i = 0; i < 512; i++)
	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3(0, 8.0f, 0), Vector3::one, Quaternion::identity);
		registry->assign<PointLight>(entity, Vector4(1, 0, 0, 1), 10.0f, 2.0f);
	}

	{
		auto entity = registry->create();
		Vector3 position = Vector3(0, 8.0f, 0);
		Vector3 scale(0.2f, 0.2f, 0.2f);
		Quaternion rotation = Quaternion::identity;
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BrickMat", "Resources/sphere.obj");
	}







	/*{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3(-10.0f, 8.0f, -10.0f), Vector3::one, Quaternion::identity);
		registry->assign<DirectionalLight>(entity, Vector3(0.7f,-0.8f,0.7f).Normalized(), Vector3(1.0f, 1.0f, 1.0f));
	}*/
}