#pragma once

#include "../Core/Math/Vector3.hpp"

struct SpotLight
{
	Vector3 color;
	float intensity;
    float cutOff;

	SpotLight(Vector3 color, float intensity, float cutOff) : color(color), intensity(intensity), cutOff(cutOff) {};
	SpotLight() : color(1.0f,1.0f,1.0f), intensity(1.0f), cutOff(12.5f) {};
};