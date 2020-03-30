#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../Physics/cycloneMath.hpp"
#include "../Core/Math/Vector3.hpp"
#include "../Core/Math/Quaternion.hpp"
#include "../Core/Math/Matrix4x4.hpp"

struct Transform
{
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;

	Transform() {};
	Transform(Vector3 p, Vector3 s, Quaternion r) : position(p), scale(s), rotation(r) {};

	Vector3 GetForward()
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		glm::mat4 rotate = glm::mat4_cast( glm::quat(rotation.w, rotation.x, rotation.y, rotation.z) );
		matrix = matrix * rotate;

		return TransformVector(matrix, Vector3(0, 0, 1));
	}

	Vector3 GetUp()
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		glm::mat4 rotate = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z) );
		matrix = matrix * rotate;

		return TransformVector(matrix, Vector3(0, 1, 0));
	}

	Vector3 GetRight()
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		glm::mat4 rotate = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z) );
		matrix = matrix * rotate;

		return TransformVector(matrix, Vector3(1, 0, 0));
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
		Vector3 position = Vector3(t.position.x, t.position.y, t.position.z);
		Vector3 scale = Vector3(t.scale.x, t.scale.y, t.scale.z);
		Quaternion rotation = Quaternion(t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z);

		Matrix4x4 scalingMatrix = Matrix4x4::Scaling(scale);
		Matrix4x4 rotationMatrix = Matrix4x4::Rotation(rotation);
		Matrix4x4 translationMatrix = Matrix4x4::Translation(position);

		Matrix4x4 transformationMatrix = translationMatrix * rotationMatrix * scalingMatrix;

		glm::mat4 matrix = glm::mat4(1.0f);
		matrix[0][0] = transformationMatrix[0];
		matrix[0][1] = transformationMatrix[1];
		matrix[0][2] = transformationMatrix[2];
		matrix[0][3] = transformationMatrix[3];
		matrix[1][0] = transformationMatrix[4];
		matrix[1][1] = transformationMatrix[5];
		matrix[1][2] = transformationMatrix[6];
		matrix[1][3] = transformationMatrix[7];
		matrix[2][0] = transformationMatrix[8];
		matrix[2][1] = transformationMatrix[9];
		matrix[2][2] = transformationMatrix[10];
		matrix[2][3] = transformationMatrix[11];
		matrix[3][0] = transformationMatrix[12];
		matrix[3][1] = transformationMatrix[13];
		matrix[3][2] = transformationMatrix[14];
		matrix[3][3] = transformationMatrix[15];

		return matrix;
	}

	static glm::mat4 ToMatrixGLMNoScale(Transform t)
	{
		Vector3 position = Vector3(t.position.x, t.position.y, t.position.z);
		Quaternion rotation = Quaternion(t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z);

		Matrix4x4 rotationMatrix = Matrix4x4::Rotation(rotation);
		Matrix4x4 translationMatrix = Matrix4x4::Translation(position);

		Matrix4x4 transformationMatrix = translationMatrix * rotationMatrix;

		glm::mat4 matrix = glm::mat4(1.0f);
		matrix[0][0] = transformationMatrix[0];
		matrix[0][1] = transformationMatrix[1];
		matrix[0][2] = transformationMatrix[2];
		matrix[0][3] = transformationMatrix[3];
		matrix[1][0] = transformationMatrix[4];
		matrix[1][1] = transformationMatrix[5];
		matrix[1][2] = transformationMatrix[6];
		matrix[1][3] = transformationMatrix[7];
		matrix[2][0] = transformationMatrix[8];
		matrix[2][1] = transformationMatrix[9];
		matrix[2][2] = transformationMatrix[10];
		matrix[2][3] = transformationMatrix[11];
		matrix[3][0] = transformationMatrix[12];
		matrix[3][1] = transformationMatrix[13];
		matrix[3][2] = transformationMatrix[14];
		matrix[3][3] = transformationMatrix[15];

		return matrix;
	}
	
	static Vector3 GetAxisVector(Transform t, int index)
	{
		glm::mat4 matrix = ToMatrixGLM(t);
		return Vector3(matrix[index][0], matrix[index][1], matrix[index][2]);
	}
};