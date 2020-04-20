#pragma once

#include "../Core/Math/Vector3.hpp"

struct DirectionalLight
{
	Vector3 direction;
	Vector3 color;

	DirectionalLight(Vector3 direction, Vector3 color) : direction(direction), color(color) {};
};