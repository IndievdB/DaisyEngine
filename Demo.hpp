#pragma once

#include <time.h>
#include "Physics/RigidBody.hpp"
#include "Physics/BoxCollider.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/PlaneCollider.hpp"
#include "Renderer/Camera.hpp"
#include "Behaviour/LuaBehaviour.hpp"

void LoadScene(std::shared_ptr<entt::registry> registry)
{
	auto material = ResourceManager::GetInstance()->GetMaterial("WhiteMat", "Resources/transform.shader");
	material->SetTexture("mainTex", "Resources/Prototype White.png");
	material = ResourceManager::GetInstance()->GetMaterial("BlueMat", "Resources/transform.shader");
	material->SetTexture("mainTex", "Resources/Prototype Blue.png");
	material = ResourceManager::GetInstance()->GetMaterial("GrayMat", "Resources/transform.shader");
	material->SetTexture("mainTex", "Resources/Prototype Gray.png");

	{
		auto entity = registry->create();
		Quaternion rotation = Quaternion(1, 0, 0, 0);
		registry->assign<Transform>(entity, Vector3(0.0f, 5.0f, -25.0f), Vector3(1, 1, 1), rotation);
		registry->assign<Camera>(entity);
		registry->assign<LuaBehaviour>(entity, "Resources/testing.lua");
	}

	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(1,0,0,0));
		registry->assign<MeshRenderer>(entity, "GrayMat", "Resources/plane.obj");
		PlaneCollider& collider = registry->assign<PlaneCollider>(entity);
		collider.normal = Vector3(0, 1, 0);
		collider.offset = 0;
	}

	srand(time(NULL));
	
	for (int i = 0; i < 5; i++)
	{
		auto entity = registry->create();
		Vector3 position = Vector3(rand() % 10 - 5, rand() % 5 + 5, rand() % 10 - 5);
		float diameter = rand() % 2 + 1.0f;
		Vector3 scale (diameter, diameter, diameter);
		Quaternion rotation = Quaternion(rand(), rand(), rand(), rand()); rotation = Normalize(rotation);
		registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BlueMat", "Resources/sphere.obj");

		RigidBody& rb = registry->assign<RigidBody>(entity);
		rb.setPosition(Vector3(position.x, position.y, position.z));
		rb.setOrientation(rotation);
		rb.setVelocity(Vector3());
		rb.setRotation(Vector3());
		float radius = diameter * 0.5f;
		float mass = 4.0f * 0.3333f * 3.1415f * radius * radius * radius; rb.setMass(mass);
		cyMatrix3 tensor; float coeff = 0.4f * mass * radius * radius;
		tensor.setInertiaTensorCoeffs(coeff, coeff, coeff); rb.setInertiaTensor(tensor);
		rb.setLinearDamping(0.95f);
		rb.setAngularDamping(0.8f);
		rb.clearAccumulators();
		rb.setAcceleration(0, -10.0f, 0);
		rb.setAwake();
		rb.calculateDerivedData();
		
		SphereCollider& collider = registry->assign<SphereCollider>(entity);
		collider.radius = 0.5f;
	}
	
	for (int i = 0; i < 5; i++)
	{
		auto entity = registry->create();
		Vector3 position = Vector3(rand() % 10 - 5, rand() % 5 + 5, rand() % 10 - 5);
		Vector3 scale = Vector3(rand() % 8 + 1.0f, rand() % 2 + 1.0f, rand() % 2 + 1.0f);
		Quaternion rotation = Quaternion(rand(), rand(), rand(), rand()); rotation = Normalize(rotation);
		registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BlueMat", "Resources/cube.obj");

		RigidBody& rb = registry->assign<RigidBody>(entity);
		rb.setPosition(Vector3(position.x, position.y, position.z));
		rb.setOrientation(rotation);
		rb.setVelocity(Vector3());
		rb.setRotation(Vector3());
		Vector3 halfSize = Vector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);
		float mass = halfSize.x * halfSize.y * halfSize.z * 8.0f; rb.setMass(mass);
		cyMatrix3 tensor; tensor.setBlockInertiaTensor(halfSize, mass); rb.setInertiaTensor(tensor);
		rb.setLinearDamping(0.95f);
		rb.setAngularDamping(0.8f);
		rb.clearAccumulators();
		rb.setAcceleration(0, -10.0f, 0);
		rb.setAwake();
		rb.calculateDerivedData();

		BoxCollider& collider = registry->assign<BoxCollider>(entity);
		collider.halfSize = Vector3(0.5f, 0.5f, 0.5f);
	}
}