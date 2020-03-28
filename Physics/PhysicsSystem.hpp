#pragma once

#include "../entt/entt.hpp"
#include "CollisionData.hpp"
#include "ContactResolver.hpp"

class PhysicsSystem
{
private:
	void UpdateRigidBodies(entt::registry& registry);
	void GenerateContacts(entt::registry& registry);
	CollisionData cData;
	ContactResolver resolver;
public:
	void RunPhysics(entt::registry& registry);
	PhysicsSystem() : resolver(2048) {};
};

