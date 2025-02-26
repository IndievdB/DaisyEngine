#ifndef Input_hpp
#define Input_hpp

#include <GLFW/glfw3.h>
#include <map>
#include "Keycodes.hpp"
#include "Math/Vector2.hpp"
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
	static bool GetLeftMouseButton();
	static bool GetRightMouseButton();
	static float GetScrollDelta();
	static Vector2 GetMousePositionDelta();
	static Vector2 GetMousePosition();

	void RegisterWindowCloseCallback();
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
	static Vector2 mousePosition;
	static Vector2 prevMousePosition;
	static float scrollDelta;
	bool mouseDidScroll;
};

#endif