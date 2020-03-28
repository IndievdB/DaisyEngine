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