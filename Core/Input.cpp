#include "Input.hpp"
#include "../Renderer/Window.hpp"
#include <iostream>

Input* Input::s_Instance;
GLFWwindow* Input::activeWindow;
glm::vec2 Input::mousePosition;
float Input::scrollDelta;

void WindowCloseCallback(GLFWwindow* window);
void SetCursorPosCallback(GLFWwindow* window, double x, double y);
void SetScrollCallback(GLFWwindow* window, double x, double y);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

Input::Input()
{
	Input::s_Instance = this;
}

void Input::Update()
{
	glfwPollEvents();

	std::map<int, int>::iterator it;

	for (it = keyStatus.begin(); it != keyStatus.end(); it++)
	{
		bool isPressed = Input::GetKey(static_cast<KeyCode>(it->first));
		if (it->first == KeyCode::WindowClose) isPressed = windowCloseButtonClicked;
		int prevStatus = it->second;

		if (isPressed && prevStatus <= 0) // first click
		{
			it->second = 1;
		}
		else if (isPressed && prevStatus == 1) // holding
		{
			it->second = 2;
		}
		else if (!isPressed && prevStatus > 0) // first release
		{
			it->second = -1;
		}
		else if (!isPressed && prevStatus == -1) // released
		{
			it->second = 0;
		}
	}

	if (!mouseDidScroll) scrollDelta = 0;

	windowCloseButtonClicked = false;
	mouseDidScroll = false;
}

void Input::SetActiveWindow(GLFWwindow* window)
{
	activeWindow = window;
	glfwSetWindowUserPointer(activeWindow, s_Instance);
	glfwSetWindowCloseCallback(activeWindow, WindowCloseCallback);
	glfwSetCursorPosCallback(activeWindow, SetCursorPosCallback);
	glfwSetScrollCallback(activeWindow, SetScrollCallback);

	glfwSetKeyCallback(activeWindow, KeyCallback);
}

bool Input::GetKey(KeyCode keycode)
{
	auto state = glfwGetKey(activeWindow, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::GetKeyDown(KeyCode keycode)
{
	int keyStatus = s_Instance->GetKeyStatus(keycode);
	return (keyStatus == 1);
}

bool Input::GetKeyUp(KeyCode keycode)
{
	int keyStatus = s_Instance->GetKeyStatus(keycode);
	return (keyStatus == -1);
}

bool Input::GetWindowCloseButton()
{
	return Input::GetKeyDown(KeyCode::WindowClose);
}

float Input::GetScrollDelta()
{
	return Input::scrollDelta;
}

int Input::GetKeyStatus(int key)
{
	auto keyStatusIterator = keyStatus.find(key);
	if (keyStatusIterator == keyStatus.end())
	{
		return -2;
	}
	else
	{
		return keyStatusIterator->second;
	}
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input* inputInstance = static_cast<Input*>(glfwGetWindowUserPointer(window));
	inputInstance->RegisterKeyCallback(key);
}

void Input::RegisterKeyCallback(int key)
{
	auto keyStatusIterator = keyStatus.find(key);
	if (keyStatusIterator == keyStatus.end())
	{
		keyStatus.emplace(key, 0);
		return;
	}
}

void WindowCloseCallback(GLFWwindow* window)
{
	Input* inputInstance = static_cast<Input*>(glfwGetWindowUserPointer(window));
	inputInstance->RegisterWindowCloseCallback();
}

void Input::RegisterWindowCloseCallback()
{
	windowCloseButtonClicked = true;
	RegisterKeyCallback(0);
}

void SetCursorPosCallback(GLFWwindow* window, double x, double y)
{
	Input* inputInstance = static_cast<Input*>(glfwGetWindowUserPointer(window));
	inputInstance->RegisterSetCursorPosCallback(x, y);
}

void Input::RegisterSetCursorPosCallback(double x, double y)
{
	Input::mousePosition = glm::vec2(x, y);
}

glm::vec2 Input::GetMousePosition()
{
	return mousePosition;
}

void SetScrollCallback(GLFWwindow* window, double x, double y)
{
	Input* inputInstance = static_cast<Input*>(glfwGetWindowUserPointer(window));
	inputInstance->RegisterSetScrollCallback(y);
}

void Input::RegisterSetScrollCallback(float delta)
{
	Input::scrollDelta = delta;
	mouseDidScroll = true;
}