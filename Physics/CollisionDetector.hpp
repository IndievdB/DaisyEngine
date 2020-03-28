#pragma once

#include "BoxCollider.hpp"
#include "SphereCollider.hpp"
#include "PlaneCollider.hpp"
#include "../Core/Transform.hpp"
#include "RigidBody.hpp"
#include "CollisionData.hpp"

class CollisionDetector
{
public:
	static unsigned SphereAndHalfSpace(const SphereCollider& sphere, const Transform& sphereTransform, RigidBody* sphereRigidBody, const PlaneCollider& plane, CollisionData& data);

	static unsigned SphereAndSphere(const SphereCollider& one, const Transform& oneTransform, RigidBody* oneRigidBody, const SphereCollider& two, const Transform& twoTransform, RigidBody* twoRigidBody, CollisionData& data);

	static unsigned BoxAndHalfSpace(const BoxCollider& box, const Transform& boxTransform, RigidBody* boxRigidBody, const PlaneCollider& plane, CollisionData& data);

	static unsigned BoxAndBox(const BoxCollider& one, const Transform& oneTransform, RigidBody* oneRigidBody, const BoxCollider& two, const Transform& twoTransform, RigidBody* twoRigidBody, CollisionData& data);

	static unsigned BoxAndSphere(const BoxCollider& box, const Transform& boxTransform, RigidBody* boxRigidBody, const SphereCollider& sphere, const Transform& sphereTransform, RigidBody* sphereRigidBody, CollisionData& data);
};
