/*#include "Camera.hpp"
#include "../Core/Input.hpp"
#include <iostream>

Camera::Camera()
{
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	Yaw = -90.0f;
	Pitch = 0.0f;
	MovementSpeed = 18.0f;
	FastSpeed = 36.0f;
	MouseSensitivity = 0.1f;
	Zoom = 45.0f;
	updateCameraVectors();
}

Camera::Camera(glm::vec3 position)
{
	Position = position;
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	Yaw = -90.0f;
	Pitch = 0.0f;
	MovementSpeed = 18.0f;
	MouseSensitivity = 0.1f;
	Zoom = 45.0f;
	updateCameraVectors();
};

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera::Camera_Movement direction, float deltaTime)
{
	float speed = Input::GetKey(KeyCode::LeftShift) ? FastSpeed : MovementSpeed;
	float velocity = speed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
	if (direction == UP)
		Position += Up * velocity;
	if (direction == DOWN)
		Position -= Up * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

void Camera::ProcessInput(float deltaTime)
{
	ProcessMouseScroll(Input::GetScrollDelta());

	glm::vec2 mousePos = Input::GetMousePosition();

	if (mousePos.x == 0 && mousePos.y == 0)
	{
		prevMousePos = mousePos;
	}
	else if (!intializedMousePos)
	{
		prevMousePos = mousePos;
		intializedMousePos = true;
	}

	float xoffset = mousePos.x - prevMousePos.x;
	float yoffset = prevMousePos.y - mousePos.y;
	prevMousePos = mousePos;

	ProcessMouseMovement(xoffset, yoffset);

	if (Input::GetKey(KeyCode::W))
		ProcessKeyboard(FORWARD, deltaTime);
	if (Input::GetKey(KeyCode::S))
		ProcessKeyboard(BACKWARD, deltaTime);
	if (Input::GetKey(KeyCode::A))
		ProcessKeyboard(LEFT, deltaTime);
	if (Input::GetKey(KeyCode::D))
		ProcessKeyboard(RIGHT, deltaTime);
	if (Input::GetKey(KeyCode::E))
		ProcessKeyboard(UP, deltaTime);
	if (Input::GetKey(KeyCode::Q))
		ProcessKeyboard(DOWN, deltaTime);
}*/