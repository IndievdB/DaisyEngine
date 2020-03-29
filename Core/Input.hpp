#ifndef Input_hpp
#define Input_hpp

#include <GLFW/glfw3.h>
#include <map>
#include "Keycodes.hpp"
#include <glm/glm.hpp>
#include <string>

class Input
{
public:
	Input();
	static void SetActiveWindow(GLFWwindow* window);
	static KeyCode StringToKeyCode(std::string input);
	static float GetDeltaTime();
	static bool GetKey(KeyCode keycode);
	static bool GetKeyDown(KeyCode keycode);
	static bool GetKeyUp(KeyCode keycode);
	static bool GetWindowCloseButton();
	static float GetScrollDelta();
	static glm::vec2 GetMousePosition();

	void RegisterWindowCloseCallback();
	void RegisterSetCursorPosCallback();
	void RegisterSetScrollCallback(float delta);
	void RegisterKeyCallback(int key);
	void RegisterSetCursorPosCallback(double x, double y);

	int GetKeyStatus(int key);
	void Update();

private:
	static std::map<std::string, KeyCode> stringToKeyCodeMap;
	static float deltaTime, previousTime;
	static Input* s_Instance;
	static GLFWwindow* activeWindow;
	std::map<int, int> keyStatus;
	int windowCloseButtonClicked;
	static glm::vec2 mousePosition;
	static float scrollDelta;
	bool mouseDidScroll;
};

#endif