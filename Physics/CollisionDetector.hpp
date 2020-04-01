#pragma once

#include "BoxCollider.hpp"
#include "SphereCollider.hpp"
#include "PlaneCollider.hpp"
#include "../Core/Transform.hpp"
#include "RigidBody.hpp"
#include "CollisionData.hpp"

#include "../Vendor/entt/entt.hpp"

class CollisionDetector
{
public:

	static unsigned SphereAndPlane(const std::shared_ptr<entt::registry> registry, const entt::entity& sphere, const entt::entity& plane, CollisionData& data);
	static unsigned SphereAndSphere(const std::shared_ptr<entt::registry> registry, const entt::entity& one, const entt::entity& two, CollisionData& data);
	static unsigned BoxAndPlane(const std::shared_ptr<entt::registry> registry, const entt::entity& box, const entt::entity& plane, CollisionData& data);
	static unsigned BoxAndBox(const std::shared_ptr<entt::registry> registry, const entt::entity& one, const entt::entity& two, CollisionData& data);
	static unsigned BoxAndSphere(const std::shared_ptr<entt::registry> registry, const entt::entity& box, const entt::entity& sphere, CollisionData& data);
};
