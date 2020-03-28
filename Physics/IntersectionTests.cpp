#include "IntersectionTests.hpp"
#include <glm/glm.hpp>


bool IntersectionTests::SphereAndHalfSpace(const SphereCollider& sphere, const Transform& sphereTransform, const PlaneCollider& plane)
{
	// Find the distance from the origin
	float ballDistance =
		glm::dot(plane.normal,
		Transform::GetAxisVector(sphereTransform, 3)) -
		sphere.radius;

	// Check for the intersection
	return ballDistance <= plane.offset;
}

bool IntersectionTests::SphereAndSphere(const SphereCollider& one, const Transform& oneTransform, const SphereCollider& two, const Transform& twoTransform)
{
	// Find the vector between the objects
	glm::vec3 midline = Transform::GetAxisVector(oneTransform, 3) - Transform::GetAxisVector(twoTransform, 3);

	// See if it is large enough.
	return (glm::length(midline)* glm::length(midline)) < (one.radius + two.radius) * (one.radius + two.radius);
}

static inline float transformToAxis(const BoxCollider& box, const Transform& boxTransform, const glm::vec3& axis)
{
	return
		box.halfSize.x * abs(glm::dot(axis, Transform::GetAxisVector(boxTransform, 0))) +
		box.halfSize.y * abs(glm::dot(axis, Transform::GetAxisVector(boxTransform, 1))) +
		box.halfSize.z * abs(glm::dot(axis, Transform::GetAxisVector(boxTransform, 2)));
}

static inline bool overlapOnAxis(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform, const glm::vec3& axis, const glm::vec3& toCentre)
{
	// Project the half-size of one onto axis
	float oneProject = transformToAxis(one, oneTransform, axis);
	float twoProject = transformToAxis(two, twoTransform, axis);

	// Project this onto the axis
	float distance = abs(glm::dot(toCentre, axis));

	// Check for overlap
	return (distance < oneProject + twoProject);
}

// This preprocessor definition is only used as a convenience
// in the boxAndBox intersection  method.
#define TEST_OVERLAP(axis) overlapOnAxis(one, oneTransform, two, twoTransform, (axis), toCentre)

bool IntersectionTests::BoxAndBox(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform)
{
	// Find the vector between the two centres
	glm::vec3 toCentre = Transform::GetAxisVector(twoTransform, 3) - Transform::GetAxisVector(oneTransform, 3);

	return (
		// Check on box one's axes first
		TEST_OVERLAP(Transform::GetAxisVector(oneTransform, 0)) &&
		TEST_OVERLAP(Transform::GetAxisVector(oneTransform, 1)) &&
		TEST_OVERLAP(Transform::GetAxisVector(oneTransform, 2)) &&

		// And on two's
		TEST_OVERLAP(Transform::GetAxisVector(twoTransform, 0)) &&
		TEST_OVERLAP(Transform::GetAxisVector(twoTransform, 1)) &&
		TEST_OVERLAP(Transform::GetAxisVector(twoTransform, 2)) &&

		// Now on the cross products
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 0), Transform::GetAxisVector(twoTransform, 0))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 0), Transform::GetAxisVector(twoTransform, 1))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 0), Transform::GetAxisVector(twoTransform, 2))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 1), Transform::GetAxisVector(twoTransform, 0))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 1), Transform::GetAxisVector(twoTransform, 1))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 1), Transform::GetAxisVector(twoTransform, 2))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 2), Transform::GetAxisVector(twoTransform, 0))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 2), Transform::GetAxisVector(twoTransform, 1))) &&
		TEST_OVERLAP(glm::cross(Transform::GetAxisVector(oneTransform, 2), Transform::GetAxisVector(twoTransform, 2)))
		);
}
#undef TEST_OVERLAP

bool IntersectionTests::BoxAndHalfSpace(const BoxCollider& box, const Transform& boxTransform, const PlaneCollider& plane)
{
	// Work out the projected radius of the box onto the plane direction
	float projectedRadius = transformToAxis(box, boxTransform, plane.normal);

	// Work out how far the box is from the origin
	float boxDistance =
		glm::dot(plane.normal, Transform::GetAxisVector(boxTransform, 3)) -
		projectedRadius;

	// Check for the intersection
	return boxDistance <= plane.offset;
}