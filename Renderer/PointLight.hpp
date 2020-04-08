#pragma once

#include "../Core/Math/Vector3.hpp"

struct PointLight
{
	Vector3 color;
	float radius;
	float intensity;

	PointLight(Vector3 color, float radius, float intensity) : color(color), radius(radius), intensity(intensity) {};
};