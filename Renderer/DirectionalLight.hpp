#pragma once

#include "../Core/Math/Vector3.hpp"

struct DirectionalLight
{
	Vector3 color;
	float intensity;

	DirectionalLight(Vector3 color, float intensity) : color(color), intensity(intensity) {};
	DirectionalLight() : color(1.0f, 1.0f, 1.0f), intensity(1.0f) {};
};