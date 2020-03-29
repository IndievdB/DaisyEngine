#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "..//Physics/cycloneMath.hpp"

struct Transform
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;

	Transform() {};
	Transform(glm::vec3 p, glm::vec3 s, glm::quat r) : position(p), scale(s), rotation(r) {};

	glm::vec3 GetForward()
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		glm::mat4 rotate = glm::mat4_cast(rotation);
		matrix = matrix * rotate;

		return TransformVector(matrix, glm::vec3(0, 0, 1));
	}

	glm::vec3 GetUp()
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		glm::mat4 rotate = glm::mat4_cast(rotation);
		matrix = matrix * rotate;

		return TransformVector(matrix, glm::vec3(0, 1, 0));
	}

	glm::vec3 GetRight()
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		glm::mat4 rotate = glm::mat4_cast(rotation);
		matrix = matrix * rotate;

		return TransformVector(matrix, glm::vec3(1, 0, 0));
	}

	void SetRotation(float pitch, float yaw, float roll)
	{
		pitch = glm::radians(pitch);
		yaw = glm::radians(yaw);
		roll = glm::radians(roll);
		rotation.x = sin(pitch / 2.0f) * cos(yaw / 2.0f) * cos(roll / 2.0f) - cos(pitch / 2.0f) * sin(yaw / 2.0f) * sin(roll / 2.0f);
		rotation.y = cos(pitch / 2.0f) * sin(yaw / 2.0f) * cos(roll / 2.0f) + sin(pitch / 2.0f) * cos(yaw / 2.0f) * sin(roll / 2.0f);
		rotation.z = cos(pitch / 2.0f) * cos(yaw / 2.0f) * sin(roll / 2.0f) - sin(pitch / 2.0f) * sin(yaw / 2.0f) * cos(roll / 2.0f);
		rotation.w = cos(pitch / 2.0f) * cos(yaw / 2.0f) * cos(roll / 2.0f) + sin(pitch / 2.0f) * sin(yaw / 2.0f) * sin(roll / 2.0f);
	}

	static glm::mat4 ToMatrixGLM(Transform t)
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, t.position);
		glm::mat4 rotate = glm::mat4_cast(t.rotation);
		matrix = matrix * rotate;
		matrix = glm::scale(matrix, t.scale);
		return matrix;
	}

	static glm::mat4 ToMatrixGLMNoScale(Transform t)
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, t.position);
		glm::mat4 rotate = glm::mat4_cast(t.rotation);
		matrix = matrix * rotate;
		return matrix;
	}
	
	static glm::vec3 GetAxisVector(Transform t, int index)
	{
		glm::mat4 matrix = ToMatrixGLM(t);
		return glm::vec3(matrix[index][0], matrix[index][1], matrix[index][2]);
	}
};