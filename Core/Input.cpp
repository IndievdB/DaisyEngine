#include "Input.hpp"
#include "../Renderer/Window.hpp"
#include <iostream>

Input* Input::s_Instance;
GLFWwindow* Input::activeWindow;
Vector2 Input::mousePosition;
Vector2 Input::prevMousePosition;
float Input::scrollDelta;
float Input::deltaTime = -1;
float Input::previousTime;

std::map<std::string, KeyCode> Input::stringToKeyCodeMap = []
{
	std::map<std::string, KeyCode> ret;
	ret["WindowClose"] = KeyCode::WindowClose;
	ret["Space"] = KeyCode::Space;
	ret["Apostrophe"] = KeyCode::Apostrophe;
	ret["Comma"] = KeyCode::Comma;
	ret["Minus"] = KeyCode::Minus;
	ret["Period"] = KeyCode::Period;
	ret["ForwardSlash"] = KeyCode::ForwardSlash;
	ret["Keypad0"] = KeyCode::Keypad0;
	ret["Keypad1"] = KeyCode::Keypad1;
	ret["Keypad2"] = KeyCode::Keypad2;
	ret["Keypad3"] = KeyCode::Keypad3;
	ret["Keypad4"] = KeyCode::Keypad4;
	ret["Keypad5"] = KeyCode::Keypad5;
	ret["Keypad6"] = KeyCode::Keypad6;
	ret["Keypad7"] = KeyCode::Keypad7;
	ret["Keypad8"] = KeyCode::Keypad8;
	ret["Keypad9"] = KeyCode::Keypad9;
	ret["Semicolon"] = KeyCode::Semicolon;
	ret["Equals"] = KeyCode::Equals;
	ret["A"] = KeyCode::A;
	ret["B"] = KeyCode::B;
	ret["C"] = KeyCode::C;
	ret["D"] = KeyCode::D;
	ret["E"] = KeyCode::E;
	ret["F"] = KeyCode::F;
	ret["G"] = KeyCode::G;
	ret["H"] = KeyCode::H;
	ret["I"] = KeyCode::I;
	ret["J"] = KeyCode::J;
	ret["K"] = KeyCode::K;
	ret["L"] = KeyCode::L;
	ret["M"] = KeyCode::M;
	ret["N"] = KeyCode::N;
	ret["O"] = KeyCode::O;
	ret["P"] = KeyCode::P;
	ret["Q"] = KeyCode::Q;
	ret["R"] = KeyCode::R;
	ret["S"] = KeyCode::S;
	ret["T"] = KeyCode::T;
	ret["U"] = KeyCode::U;
	ret["V"] = KeyCode::V;
	ret["W"] = KeyCode::W;
	ret["X"] = KeyCode::X;
	ret["Y"] = KeyCode::Y;
	ret["Z"] = KeyCode::Z;
	ret["LeftBracket"] = KeyCode::LeftBracket;
	ret["BackSlash"] = KeyCode::BackSlash;
	ret["RightBracket"] = KeyCode::RightBracket;
	ret["GraveAccent"] = KeyCode::GraveAccent;
	ret["World1"] = KeyCode::World1;
	ret["World2"] = KeyCode::World2;
	ret["Escape"] = KeyCode::Escape;
	ret["Enter"] = KeyCode::Enter;
	ret["Tab"] = KeyCode::Tab;
	ret["Backspace"] = KeyCode::Backspace;
	ret["Insert"] = KeyCode::Insert;
	ret["Delete"] = KeyCode::Delete;
	ret["Right"] = KeyCode::Right;
	ret["Left"] = KeyCode::Left;
	ret["Down"] = KeyCode::Down;
	ret["Up"] = KeyCode::Up;
	ret["PageUp"] = KeyCode::PageUp;
	ret["PageDown"] = KeyCode::PageDown;
	ret["Home"] = KeyCode::Home;
	ret["End"] = KeyCode::End;
	ret["CapsLock"] = KeyCode::CapsLock;
	ret["ScrollLock"] = KeyCode::ScrollLock;
	ret["NumLock"] = KeyCode::NumLock;
	ret["PrintScreen"] = KeyCode::PrintScreen;
	ret["Pause"] = KeyCode::Pause;
	ret["F1"] = KeyCode::F1;
	ret["F2"] = KeyCode::F2;
	ret["F3"] = KeyCode::F3;
	ret["F4"] = KeyCode::F4;
	ret["F5"] = KeyCode::F5;
	ret["F6"] = KeyCode::F6;
	ret["F7"] = KeyCode::F7;
	ret["F8"] = KeyCode::F8;
	ret["F9"] = KeyCode::F9;
	ret["F10"] = KeyCode::F10;
	ret["F11"] = KeyCode::F11;
	ret["F12"] = KeyCode::F12;
	ret["F13"] = KeyCode::F13;
	ret["F14"] = KeyCode::F14;
	ret["F15"] = KeyCode::F15;
	ret["KP_0"] = KeyCode::KP_0;
	ret["KP_1"] = KeyCode::KP_1;
	ret["KP_2"] = KeyCode::KP_2;
	ret["KP_3"] = KeyCode::KP_3;
	ret["KP_4"] = KeyCode::KP_4;
	ret["KP_5"] = KeyCode::KP_5;
	ret["KP_6"] = KeyCode::KP_6;
	ret["KP_7"] = KeyCode::KP_7;
	ret["KP_8"] = KeyCode::KP_8;
	ret["KP_9"] = KeyCode::KP_9;
	ret["KP_Decimal"] = KeyCode::KP_Decimal;
	ret["KP_Divide"] = KeyCode::KP_Divide;
	ret["KP_Multiply"] = KeyCode::KP_Multiply;
	ret["KP_Subtract"] = KeyCode::KP_Subtract;
	ret["KP_Add"] = KeyCode::KP_Add;
	ret["KP_Enter"] = KeyCode::KP_Enter;
	ret["KP_Equal"] = KeyCode::KP_Equal;
	ret["LeftShift"] = KeyCode::LeftShift;
	ret["LeftControl"] = KeyCode::LeftControl;
	ret["LeftAlt"] = KeyCode::LeftAlt;
	ret["LeftSuper"] = KeyCode::LeftSuper;
	ret["RightShift"] = KeyCode::RightShift;
	ret["RightControl"] = KeyCode::RightControl;
	ret["RightAlt"] = KeyCode::RightAlt;
	ret["RightSuper"] = KeyCode::RightSuper;
	ret["Menu"] = KeyCode::Menu;
	return ret;
}();

KeyCode Input::StringToKeyCode(std::string input)
{
	return stringToKeyCodeMap[input];
}


void WindowCloseCallback(GLFWwindow* window);
void SetCursorPosCallback(GLFWwindow* window, double x, double y);
void SetScrollCallback(GLFWwindow* window, double x, double y);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

Input::Input()
{
	Input::s_Instance = this;
	previousTime = glfwGetTime();
}

void Input::Update()
{
	prevMousePosition = mousePosition;
	glfwPollEvents();

	float currentTime = glfwGetTime();
	deltaTime = (deltaTime == -1) ? 0.0f : currentTime - previousTime;
	previousTime = currentTime;

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

float Input::GetDeltaTime()
{
	return deltaTime;
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

bool Input::GetLeftMouseButton()
{
	return (glfwGetMouseButton(activeWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

bool Input::GetRightMouseButton()
{
	return (glfwGetMouseButton(activeWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
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
	Input::mousePosition = Vector2(x, y);
}

Vector2 Input::GetMousePositionDelta()
{
	return mousePosition - prevMousePosition;
}

Vector2 Input::GetMousePosition()
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