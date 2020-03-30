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
	static unsigned SphereAndHalfSpace(const SphereCollider& sphere, Transform& sphereTransform, RigidBody* sphereRigidBody, const PlaneCollider& plane, CollisionData& data);

	static unsigned SphereAndSphere(const SphereCollider& one, Transform& oneTransform, RigidBody* oneRigidBody, const SphereCollider& two, Transform& twoTransform, RigidBody* twoRigidBody, CollisionData& data);

	static unsigned BoxAndHalfSpace(const BoxCollider& box, Transform& boxTransform, RigidBody* boxRigidBody, const PlaneCollider& plane, CollisionData& data);

	static unsigned BoxAndBox(const BoxCollider& one, Transform& oneTransform, RigidBody* oneRigidBody, const BoxCollider& two, Transform& twoTransform, RigidBody* twoRigidBody, CollisionData& data);

	static unsigned BoxAndSphere(const BoxCollider& box, Transform& boxTransform, RigidBody* boxRigidBody, const SphereCollider& sphere, Transform& sphereTransform, RigidBody* sphereRigidBody, CollisionData& data);
};
