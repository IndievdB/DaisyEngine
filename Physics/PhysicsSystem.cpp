#include "PhysicsSystem.hpp"
#include "../Core/Transform.hpp"
#include "RigidBody.hpp"
#include "BoxCollider.hpp"
#include "SphereCollider.hpp"
#include "PlaneCollider.hpp"
#include <iostream>
#include "../Vendor/entt/entt.hpp"
#include "CollisionDetector.hpp"
//#include "../Behaviour/LuaBehaviour.hpp"

void PhysicsSystem::RunPhysics(std::shared_ptr<entt::registry> registry)
{
	UpdateTriggers(registry);
	UpdateRigidBodies(registry);
	GenerateContacts(registry);
	resolver.resolveContacts(cData.contactArray, cData.contactCount, 0.01f);
}

void PhysicsSystem::UpdateTriggers(std::shared_ptr<entt::registry> registry)
{
	registry->view<PlaneCollider>().each([](auto& collider) { collider.triggerData.NextFrame(); });
	registry->view<BoxCollider>().each([](auto& collider) { collider.triggerData.NextFrame(); });
	registry->view<SphereCollider>().each([](auto& collider) { collider.triggerData.NextFrame(); });
}

void PhysicsSystem::UpdateRigidBodies(std::shared_ptr<entt::registry> registry)
{
	registry->view<Transform, RigidBody>().each([](auto& transform, auto& rigidBody)
	{
		rigidBody.calculateDerivedData(transform);
		rigidBody.integrate(transform, 0.01f);
	});
}

void PhysicsSystem::GenerateContacts(std::shared_ptr<entt::registry> registry)
{
	cData.reset(256);
	cData.friction = 0.9f;
	cData.restitution = 0.6f;
	cData.tolerance = 0.1f;

	auto boxTypes = { entt::type_info<Transform>::id(), entt::type_info<BoxCollider>::id() };
	auto boxes = registry->runtime_view(std::cbegin(boxTypes), std::cend(boxTypes));

	auto sphereTypes = { entt::type_info<Transform>::id(), entt::type_info<SphereCollider>::id() };
	auto spheres = registry->runtime_view(std::cbegin(sphereTypes), std::cend(sphereTypes));
	
	auto planes = registry->view<Transform, PlaneCollider>();

	// Check all boxes against...
	for (auto box = boxes.begin(); box != boxes.end(); ++box)
	{
		// all planes
		for (auto plane = planes.begin(); plane != planes.end(); ++plane)
		{
			if (!cData.hasMoreContacts()) return;
			CollisionDetector::BoxAndPlane(registry, *box, *plane, cData);
		}

		// all other boxes
		for (auto otherBox = std::next(box); otherBox != boxes.end(); ++otherBox)
		{
			if (!cData.hasMoreContacts()) return;
			CollisionDetector::BoxAndBox(registry, *box, *otherBox, cData);
		}

		// all spheres
		for (auto sphere = spheres.begin(); sphere != spheres.end(); ++sphere)
		{
			if (!cData.hasMoreContacts()) return;
			CollisionDetector::BoxAndSphere(registry, *box, *sphere, cData);
		}
	}

	// Check all spheres against...
	for (auto sphere = spheres.begin(); sphere != spheres.end(); ++sphere)
	{
		// all planes
		for (auto plane = planes.begin(); plane != planes.end(); ++plane)
		{
			if (!cData.hasMoreContacts()) return;
			CollisionDetector::SphereAndPlane(registry, *sphere, *plane, cData);
		}

		// all other spheres
		for (auto otherSphere = std::next(sphere); otherSphere != spheres.end(); ++otherSphere)
		{
			if (!cData.hasMoreContacts()) return;
			CollisionDetector::SphereAndSphere(registry, *sphere, *otherSphere, cData);
		}
	}
}