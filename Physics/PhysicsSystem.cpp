#include "PhysicsSystem.hpp"
#include "../Core/Transform.hpp"
#include "RigidBody.hpp"
#include "BoxCollider.hpp"
#include "SphereCollider.hpp"
#include "PlaneCollider.hpp"
#include <iostream>
#include "../Vendor/entt/entt.hpp"
#include "CollisionDetector.hpp"

void PhysicsSystem::RunPhysics(entt::registry& registry)
{
	//if (cData.contactCount > 0)
	//	return;
		
	UpdateRigidBodies(registry);

	registry.view<Transform, RigidBody>().each([](auto& transform, auto& rigidBody)
	{
		transform.position = rigidBody.getPosition();
		transform.rotation = rigidBody.getOrientation();
	});

	GenerateContacts(registry);

	resolver.resolveContacts(cData.contactArray, cData.contactCount, 0.01f);

	registry.view<Transform, RigidBody>().each([](auto& transform, auto& rigidBody)
	{
		transform.position = rigidBody.getPosition();
		transform.rotation = rigidBody.getOrientation();
	});
}

void PhysicsSystem::UpdateRigidBodies(entt::registry& registry)
{
	registry.view<Transform, RigidBody>().each([](auto& transform, auto& rigidBody)
	{
		rigidBody.setPosition(transform.position);
		rigidBody.setOrientation(transform.rotation);
		rigidBody.calculateDerivedData();
		rigidBody.integrate(0.01f);
	});
}

void PhysicsSystem::GenerateContacts(entt::registry& registry)
{
	cData.reset(256);
	cData.friction = 0.9f;
	cData.restitution = 0.6f;
	cData.tolerance = 0.1f;

	auto boxTypes = { entt::type_info<Transform>::id(), entt::type_info<BoxCollider>::id() };
	auto boxes = registry.runtime_view(std::cbegin(boxTypes), std::cend(boxTypes));

	auto sphereTypes = { entt::type_info<Transform>::id(), entt::type_info<SphereCollider>::id() };
	auto spheres = registry.runtime_view(std::cbegin(sphereTypes), std::cend(sphereTypes));
	
	auto planes = registry.view<Transform, PlaneCollider>();

	// Check all boxes against...
	for (auto box = boxes.begin(); box != boxes.end(); ++box)
	{
		Transform& transform = registry.get<Transform>(*box);
		BoxCollider& collider = registry.get<BoxCollider>(*box);
		RigidBody* rigidBody = registry.try_get<RigidBody>(*box);

		// all planes
		for (auto plane = planes.begin(); plane != planes.end(); ++plane)
		{
			if (!cData.hasMoreContacts()) return;

			CollisionDetector::BoxAndHalfSpace(collider, transform, rigidBody, registry.get<PlaneCollider>(*plane), cData);
			// CollisionDetector.boxAndHalfSpace(box, plane, ref cData);
		}

		// all other boxes
		for (auto otherBox = std::next(box); otherBox != boxes.end(); ++otherBox)
		{
			if (!cData.hasMoreContacts()) return;

			Transform& otherTransform = registry.get<Transform>(*otherBox);
			BoxCollider& otherCollider = registry.get<BoxCollider>(*otherBox);
			RigidBody* otherRigidBody = registry.try_get<RigidBody>(*otherBox);

			CollisionDetector::BoxAndBox(collider, transform, rigidBody, otherCollider, otherTransform, otherRigidBody, cData);

			// CollisionDetector.boxAndBox(box, other, ref cData);
		}

		// all spheres
		for (auto sphere = spheres.begin(); sphere != spheres.end(); ++sphere)
		{
			if (!cData.hasMoreContacts()) return;

			Transform& sphereTransform = registry.get<Transform>(*sphere);
			SphereCollider& sphereCollider = registry.get<SphereCollider>(*sphere);
			RigidBody* sphereRigidBody = registry.try_get<RigidBody>(*sphere);

			CollisionDetector::BoxAndSphere(collider, transform, rigidBody, sphereCollider, sphereTransform, sphereRigidBody, cData);
			//CollisionDetector.boxAndSphere(box, other, ref cData);
		}
	}

	// Check all spheres against...
	for (auto sphere = spheres.begin(); sphere != spheres.end(); ++sphere)
	{
		Transform& transform = registry.get<Transform>(*sphere);
		SphereCollider& collider = registry.get<SphereCollider>(*sphere);
		RigidBody* rigidBody = registry.try_get<RigidBody>(*sphere);

		// all planes
		for (auto plane = planes.begin(); plane != planes.end(); ++plane)
		{
			if (!cData.hasMoreContacts()) return;

			CollisionDetector::SphereAndHalfSpace(collider, transform, rigidBody, registry.get<PlaneCollider>(*plane), cData);
			//CollisionDetector.sphereAndHalfSpace(ball, plane, ref cData);
		}

		// all other spheres
		for (auto otherSphere = std::next(sphere); otherSphere != spheres.end(); ++otherSphere)
		{
			if (!cData.hasMoreContacts()) return;

			Transform& otherTransform = registry.get<Transform>(*otherSphere);
			SphereCollider& otherCollider = registry.get<SphereCollider>(*otherSphere);
			RigidBody* otherRigidBody = registry.try_get<RigidBody>(*otherSphere);

			CollisionDetector::SphereAndSphere(collider, transform, rigidBody, otherCollider, otherTransform, otherRigidBody, cData);
			//CollisionDetector.sphereAndSphere(ball, other, ref cData);
		}
	}
}