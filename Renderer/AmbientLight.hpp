#pragma once

#include "../Core/Math/Vector3.hpp"

struct AmbientLight
{
	Vector3 color;
	float intensity;

	AmbientLight(Vector3 color, float intensity) : color(color), intensity(intensity) {};
	AmbientLight() : color(1.0f, 1.0f, 1.0f), intensity(0.2f) {};
};