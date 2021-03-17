#pragma once

#include "../Core/Math/Vector3.hpp"
#include "../Core/Math/Quaternion.hpp"
#include "../Core/Math/Matrix3x3.hpp"
#include "../Core/Math/Matrix4x4.hpp"
#include "../Core/Math/Mathf.hpp"
#include <iostream>

struct Transform
{
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;

	Transform() : scale(1.0f, 1.0f, 1.0f) {};
	Transform(Vector3 p, Vector3 s, Quaternion r) : position(p), scale(s), rotation(r) {};

	Vector3 GetForward()
	{
		return Matrix3x3(rotation) * Vector3::forward;
	}

	Vector3 GetUp()
	{
		return Matrix3x3(rotation) * Vector3::up;
	}

	Vector3 GetRight()
	{
		return Matrix3x3(rotation) * Vector3::right;
	}
};