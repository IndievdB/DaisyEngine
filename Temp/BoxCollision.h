#pragma once

#include "Plane.h"

class BoxCollision
{
public:
	BoxCollision(Plane faces[6]);
	BoxCollision() {}
	~BoxCollision();

	const bool SphereInside(const Vector3& spherePosition, const float& radius) const;
	const bool SphereIntersecting(const Vector3& spherePosition, const float& radius) const;

private:
	Plane faces[6];

	enum class Faces;
};

