#include "IntersectionTests.hpp"

bool IntersectionTests::SphereAndHalfSpace(const SphereCollider& sphere, const Transform& sphereTransform, const PlaneCollider& plane)
{
	// Find the distance from the origin
	float ballDistance = Vector3::Dot(plane.normal, Matrix4x4::Transformation(sphereTransform).GetColumn(3)) - sphere.radius;

	// Check for the intersection
	return ballDistance <= plane.offset;
}

bool IntersectionTests::SphereAndSphere(const SphereCollider& one, const Transform& oneTransform, const SphereCollider& two, const Transform& twoTransform)
{
	// Find the vector between the objects
	Vector3 midline = oneTransform.position - twoTransform.position;

	// See if it is large enough.
	return (midline.LengthSquared()) < (one.radius + two.radius) * (one.radius + two.radius);
}

static inline float transformToAxis(const BoxCollider& box, const Transform& boxTransform, const Vector3& axis)
{
	return
		box.halfSize.x * abs(Vector3::Dot(axis, Matrix4x4::Transformation(boxTransform).GetColumn(0))) +
		box.halfSize.y * abs(Vector3::Dot(axis, Matrix4x4::Transformation(boxTransform).GetColumn(1))) +
		box.halfSize.z * abs(Vector3::Dot(axis, Matrix4x4::Transformation(boxTransform).GetColumn(2)));
}

static inline bool overlapOnAxis(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform, const Vector3& axis, const Vector3& toCentre)
{
	// Project the half-size of one onto axis
	float oneProject = transformToAxis(one, oneTransform, axis);
	float twoProject = transformToAxis(two, twoTransform, axis);

	// Project this onto the axis
	float distance = abs(Vector3::Dot(toCentre, axis));

	// Check for overlap
	return (distance < oneProject + twoProject);
}

// This preprocessor definition is only used as a convenience
// in the boxAndBox intersection  method.
#define TEST_OVERLAP(axis) overlapOnAxis(one, oneTransform, two, twoTransform, (axis), toCentre)

bool IntersectionTests::BoxAndBox(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform)
{
	// Find the vector between the two centres
	Vector3 toCentre = twoTransform.position - oneTransform.position;

	return (
		// Check on box one's axes first
		TEST_OVERLAP(Matrix4x4::Transformation(oneTransform).GetColumn(0)) &&
		TEST_OVERLAP(Matrix4x4::Transformation(oneTransform).GetColumn(1)) &&
		TEST_OVERLAP(Matrix4x4::Transformation(oneTransform).GetColumn(2)) &&

		// And on two's
		TEST_OVERLAP(Matrix4x4::Transformation(twoTransform).GetColumn(0)) &&
		TEST_OVERLAP(Matrix4x4::Transformation(twoTransform).GetColumn(1)) &&
		TEST_OVERLAP(Matrix4x4::Transformation(twoTransform).GetColumn(2)) &&

		// Now on the cross products
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(0), Matrix4x4::Transformation(twoTransform).GetColumn(0))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(0), Matrix4x4::Transformation(twoTransform).GetColumn(1))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(0), Matrix4x4::Transformation(twoTransform).GetColumn(2))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(1), Matrix4x4::Transformation(twoTransform).GetColumn(0))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(1), Matrix4x4::Transformation(twoTransform).GetColumn(1))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(1), Matrix4x4::Transformation(twoTransform).GetColumn(2))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(2), Matrix4x4::Transformation(twoTransform).GetColumn(0))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(2), Matrix4x4::Transformation(twoTransform).GetColumn(1))) &&
		TEST_OVERLAP(Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(2), Matrix4x4::Transformation(twoTransform).GetColumn(2)))
		);
}
#undef TEST_OVERLAP

bool IntersectionTests::BoxAndHalfSpace(const BoxCollider& box, const Transform& boxTransform, const PlaneCollider& plane)
{
	// Work out the projected radius of the box onto the plane direction
	float projectedRadius = transformToAxis(box, boxTransform, plane.normal);

	// Work out how far the box is from the origin
	float boxDistance = Vector3::Dot(plane.normal, boxTransform.position) - projectedRadius;

	// Check for the intersection
	return boxDistance <= plane.offset;
}