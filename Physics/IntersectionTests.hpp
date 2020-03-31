#pragma once

#include "BoxCollider.hpp"
#include "SphereCollider.hpp"
#include "PlaneCollider.hpp"
#include "../Core/Transform.hpp"

class IntersectionTests
{
public:
	static bool SphereAndHalfSpace(const SphereCollider& sphere, const Transform& sphereTransform, const PlaneCollider& plane);

	static bool SphereAndSphere(const SphereCollider& one, const Transform& oneTransform, const SphereCollider& two, const Transform& twoTransform);

	static bool BoxAndBox(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform);

	static bool BoxAndHalfSpace(const BoxCollider& box, const Transform& boxTransform, const PlaneCollider& plane);

	static bool BoxAndSphere(const BoxCollider& box, const Transform& boxTransform, const SphereCollider& sphere, const Transform& sphereTransform);
};
