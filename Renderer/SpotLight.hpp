#pragma once

#include "../Core/Math/Vector3.hpp"

struct SpotLight
{
	Vector3 color;
	float intensity;
    float cutOff;
	float range;

	SpotLight(Vector3 color, float intensity, float cutOff, float range) : color(color), intensity(intensity), cutOff(cutOff), range(range) {};
	SpotLight() : color(1.0f, 1.0f, 1.0f), intensity(1.0f), cutOff(12.5f), range(15.0f) {};
};