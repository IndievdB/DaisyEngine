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

#include "Renderer/PointLight.hpp"
#include "Renderer/AmbientLight.hpp"
#include "Renderer/DirectionalLight.hpp"
#include "Renderer/SpotLight.hpp"

void LoadScene(std::shared_ptr<entt::registry> registry)
{
	{
		auto entity = registry->create();
		Quaternion rotation = Quaternion(1, 0, 0, 0);
		registry->assign<EntityName>(entity, "Main Camera");
		registry->assign<Transform>(entity, Vector3(0.0f, 5.0f, -25.0f), Vector3::one, rotation);
		registry->assign<Camera>(entity);
		registry->assign<LuaBehaviour>(entity, "Resources/FreeCam.lua");
	}

	/*{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, registry, "Ambient Light");
		registry->assign<Transform>(entity, Vector3::up * 3, Vector3::one * 0.25f, Quaternion::identity);
		registry->assign<AmbientLight>(entity);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/DefaultWhiteGrid.material", "Resources/Engine/Meshes/DefaultSphere.obj");
	}*/

	/*{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, registry, "Point Light");
		registry->assign<Transform>(entity, Vector3::up * 3, Vector3::one * 0.25f, Quaternion::identity);
		registry->assign<PointLight>(entity);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/DefaultWhiteGrid.material", "Resources/Engine/Meshes/DefaultSphere.obj");
	}*/

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, registry, "Point Light");
		registry->assign<Transform>(entity, Vector3::up * 8, Vector3::one * 0.25f, Quaternion::identity);
		registry->assign<PointLight>(entity, Vector4::one, 15.0f, 5.0f, true);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/DefaultWhiteGrid.material", "Resources/Engine/Meshes/DefaultSphere.obj", false);
	}

	for (int i = 0; i < 10; i++)
	{
		{
			float x = rand() % 30 - 15;
			float z = rand() % 30 - 15;
			float y = rand() % 10;

			float r = (double)rand() / (RAND_MAX);
			float g = (double)rand() / (RAND_MAX);
			float b = (double)rand() / (RAND_MAX);

			auto entity = registry->create();
			registry->assign<EntityName>(entity, registry, "Point Light");
			registry->assign<Transform>(entity, Vector3(x,y,z), Vector3::one * 0.25f, Quaternion::identity);
			registry->assign<PointLight>(entity, Vector4(r,g,b,1), 15.0f, 5.0f, false);
			registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/DefaultWhiteGrid.material", "Resources/Engine/Meshes/DefaultSphere.obj", false);
		}
	}

	/*{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, registry, "Directional Light");
		registry->assign<Transform>(entity, Vector3::up * 3, Vector3::one * 0.25f, Quaternion(0.523599f, 0.0f, 0.0f));
		registry->assign<DirectionalLight>(entity);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/DefaultWhiteGrid.material", "Resources/Engine/Meshes/DefaultArrow.obj");
	}*/

	/*
	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Cube");
		Transform transform = registry->assign<Transform>(entity, Vector3::zero, Vector3::one, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/LambertNoShadows.material", "Resources/Engine/Meshes/DefaultCube.obj");
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Min");
		Transform transform = registry->assign<Transform>(entity, Vector3(-0.5f, -0.5f, -0.5f), Vector3::one * 0.5f, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/LambertNoShadows.material", "Resources/Engine/Meshes/DefaultSphere.obj");
	}

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Max");
		Transform transform = registry->assign<Transform>(entity, Vector3(0.5f, 0.5f, 0.5f), Vector3::one * 0.5f, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/LambertNoShadows.material", "Resources/Engine/Meshes/DefaultSphere.obj");
	}*/

	{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Ground Plane");
		registry->assign<Transform>(entity, Vector3::zero, Vector3::one, Quaternion::identity);
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/Lambert.material", "Resources/plane.obj");
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
		registry->assign<MeshRenderer>(entity, "Resources/Engine/Materials/Lambert.material", "Resources/cube.obj");
		//registry->assign<LuaBehaviour>(entity, "Resources/CollisionTest.lua");

		RigidBody& rb = registry->assign<RigidBody>(entity);
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
		rb.calculateDerivedData(transform);

		BoxCollider& collider = registry->assign<BoxCollider>(entity);
		collider.halfSize = Vector3(0.5f, 0.5f, 0.5f);
		//collider.isTrigger = true;
	}

	/*{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, " Point Light");
		Transform transform = registry->assign<Transform>(entity, Vector3(0, 2.5f, 0), Vector3::one, Quaternion::identity);
		registry->assign<PointLight>(entity);
	}*/

	/*{
		auto entity = registry->create();
		registry->assign<EntityName>(entity, "Directional Light");
		//Transform transform = registry->assign<Transform>(entity, Vector3(0, 2.5f, 0), Vector3::one, Quaternion::identity);
		registry->assign<DirectionalLight>(entity, Vector3(0,-1,0), Vector3(1.0f, 1.0f, 1.0f));
	}*/

	/*
	auto arialFont = ResourceManager::GetInstance()->GetFont("Resources/fonts/arial.ttf");
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

	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3(1.0f, 25.0f, 0.0f), Vector3::one, Quaternion::identity);
		registry->assign<TextMesh>(entity, "This is sample text", 1.0f, Vector3(0.5f, 0.8f, 0.2f), arialFont);
	}

	{
		auto entity = registry->create();
		registry->assign<Transform>(entity, Vector3(54.0f, 550.0f, 0.0f), Vector3::one, Quaternion::identity);
		registry->assign<TextMesh>(entity, "(C) LearnOpenGL.com", 0.6f, Vector3(0.3f, 0.7f, 0.9f), arialFont);
	}

	srand(time(NULL));
	
	for (int i = 0; i < 5; i++)
	{
		auto entity = registry->create();
		Vector3 position = Vector3(rand() % 8 - 4, rand() % 5 + 5, rand() % 8 - 4);
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
	
	for (int i = 0; i < 5; i++)
	{
		auto entity = registry->create();
		Vector3 position = Vector3(rand() % 8 - 4, rand() % 5 + 5, rand() % 8 - 4);
		Vector3 scale = Vector3(rand() % 4 + 1.0f, rand() % 2 + 1.0f, rand() % 2 + 1.0f);
		Quaternion rotation = Quaternion(rand(), rand(), rand(), rand()); rotation.Normalize();
		Transform transform = registry->assign<Transform>(entity, position, scale, rotation);
		registry->assign<MeshRenderer>(entity, "BlueMat", "Resources/cube.obj");
		//registry->assign<LuaBehaviour>(entity, "Resources/CollisionTest.lua");

		RigidBody& rb = registry->assign<RigidBody>(entity);
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
		rb.calculateDerivedData(transform);

		BoxCollider& collider = registry->assign<BoxCollider>(entity);
		collider.halfSize = Vector3(0.5f, 0.5f, 0.5f);
		//collider.isTrigger = true;
	}*/
}