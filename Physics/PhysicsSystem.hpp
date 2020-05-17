#pragma once

#include "../Vendor/entt/entt.hpp"
#include "CollisionData.hpp"
#include "ContactResolver.hpp"

class PhysicsSystem
{
private:
	void UpdateRigidBodies(std::shared_ptr<entt::registry> registry);
	void GenerateContacts(std::shared_ptr<entt::registry> registry);
	void UpdateTriggers(std::shared_ptr<entt::registry> registry);
	CollisionData cData;
	ContactResolver resolver;
public:
	void RunPhysics(std::shared_ptr<entt::registry> registry);
	PhysicsSystem() : resolver(2048) {};
};

