#pragma once

#include "../Core/Math/Vector3.hpp"
#include "TriggerData.hpp"

class BoxCollider
{
public:
	Vector3 halfSize;
	bool isTrigger;
	TriggerData triggerData;
};

