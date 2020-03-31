#pragma once

#include "../Core/Math/Vector3.hpp"

class PlaneCollider
{
public:
	Vector3 normal;
	float offset;
	bool isTrigger;
};
