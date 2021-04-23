#pragma once

#include "../Core/Math/Vector4.hpp"

struct PointLight
{
	Vector4 color;
	float radius;
	float intensity;
	bool castsShadows;

	PointLight(Vector4 color, float radius, float intensity, bool castsShadows) : color(color), radius(radius), intensity(intensity), castsShadows(castsShadows) {};
	PointLight() : color(1.0f,1.0f,1.0f,1.0f), radius(4.0f), intensity(4.0f), castsShadows(false) {};
};