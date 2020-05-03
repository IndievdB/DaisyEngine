#pragma once

#include <vector>

struct Camera
{
	Camera(float fov, float near, float far) : fov(fov), nearPlane(near), farPlane(far) {};
	Camera() : fov(45.0f), nearPlane(1.0f), farPlane(100.0f) {};
	float fov;
	float nearPlane;
	float farPlane;
};