#pragma once

#include <vector>

struct Camera
{
	Camera(float fov, float near, float far) : fov(fov), nearPlane(near), farPlane(far) {};
	Camera() : fov(45.0f), nearPlane(0.1f), farPlane(1000.0f) {};
	float fov;
	float nearPlane;
	float farPlane;
};