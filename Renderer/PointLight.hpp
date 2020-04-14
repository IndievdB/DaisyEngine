#pragma once

#include "../Core/Math/Vector4.hpp"

struct PointLight
{
	Vector4 color;
	float radius;
	float intensity;

	PointLight(Vector4 color, float radius, float intensity) : color(color), radius(radius), intensity(intensity) {};
};