#pragma once

#include "../Core/Math/Vector3.hpp"
#include "TriggerData.hpp"

class PlaneCollider
{
public:
	Vector3 normal;
	float offset;
	bool isTrigger;
	TriggerData triggerData;
};
