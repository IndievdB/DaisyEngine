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

	srand(time(NULL));
	
	for (int i = 0; i < 1; i++)
	{
		auto entity = registry->create();
		Vector3 position = Vector3(0, rand() % 5 + 15, 0);
		float diameter = rand() % 2 + 1.0f;
		Vector3 scale (diameter, diameter, diameter);
		Quaternion rotation = Quaternion(rand(), rand(), rand(), rand()); rotation.Normalize();
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BlueMat", "Resources/sphere.obj");

		RigidBody& rb = registry->assign<RigidBody>(entity);
		float radius = diameter * 0.5f;
		float mass = 4.0f * 0.3333f * 3.1415f * radius * radius * radius; rb.setMass(mass);
		Matrix3x3 tensor; float coeff = 0.4f * mass * radius * radius;
		tensor.SetDiagonal(coeff, coeff, coeff); rb.setInertiaTensor(tensor);
		rb.setLinearDamping(0.95f);
		rb.setAngularDamping(0.8f);
		rb.clearAccumulators();
		rb.setAcceleration(0, -10.0f, 0);
		rb.setAwake();
		rb.calculateDerivedData(transform);
		
		SphereCollider& collider = registry->assign<SphereCollider>(entity);
		collider.radius = 0.5f;
	}
	
	for (int i = 0; i < 1; i++)
	{
		auto entity = registry->create();
		Vector3 position = Vector3(0, rand() % 5 + 1, 0);
		Vector3 scale = Vector3(rand() % 4 + 1.0f, rand() % 2 + 1.0f, rand() % 2 + 1.0f);
		Quaternion rotation = Quaternion(rand(), rand(), rand(), rand()); rotation.Normalize();
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BlueMat", "Resources/cube.obj");

		/*RigidBody& rb = registry->assign<RigidBody>(entity);
		Vector3 halfSize = Vector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);
		float mass = halfSize.x * halfSize.y * halfSize.z * 8.0f; rb.setMass(mass);

		Matrix3x3 tensor;
		Vector3 squares = Vector3(halfSize.x * halfSize.x, halfSize.y * halfSize.y, halfSize.z * halfSize.z);
		tensor.SetDiagonal(0.3f * mass * (squares.y + squares.z), 0.3f * mass * (squares.x + squares.z), 0.3f * mass * (squares.x + squares.y));
		rb.setInertiaTensor(tensor);

		rb.setLinearDamping(0.95f);
		rb.setAngularDamping(0.8f);
		rb.clearAccumulators();
		rb.setAcceleration(0, -10.0f, 0);
		rb.setAwake();
		rb.calculateDerivedData(transform);*/

		BoxCollider& collider = registry->assign<BoxCollider>(entity);
		collider.halfSize = Vector3(0.5f, 0.5f, 0.5f);
		collider.isTrigger = true;
	}
}