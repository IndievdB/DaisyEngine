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

	Transform() {};
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

	void SetRotation(float pitch, float yaw, float roll)
	{
		pitch = pitch * kDegToRad;
		yaw = yaw * kDegToRad;
		roll = roll * kDegToRad;
		rotation.x = sin(pitch / 2.0f) * cos(yaw / 2.0f) * cos(roll / 2.0f) - cos(pitch / 2.0f) * sin(yaw / 2.0f) * sin(roll / 2.0f);
		rotation.y = cos(pitch / 2.0f) * sin(yaw / 2.0f) * cos(roll / 2.0f) + sin(pitch / 2.0f) * cos(yaw / 2.0f) * sin(roll / 2.0f);
		rotation.z = cos(pitch / 2.0f) * cos(yaw / 2.0f) * sin(roll / 2.0f) - sin(pitch / 2.0f) * sin(yaw / 2.0f) * cos(roll / 2.0f);
		rotation.w = cos(pitch / 2.0f) * cos(yaw / 2.0f) * cos(roll / 2.0f) + sin(pitch / 2.0f) * sin(yaw / 2.0f) * sin(roll / 2.0f);
	}
};