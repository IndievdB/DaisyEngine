#ifndef Window_hpp
#define Window_hpp

#include <functional>

struct GLFWwindow;

class Window
{
public:
	Window(const char* title, float width, float height);
	~Window();
	void ProcessInput();
	bool isOpen;
	void Clear();
	void SwapBuffers();
	void ResetDimensions();
	GLFWwindow* GetGLFWWindow();
	static Window* GetInstance();
	float GetViewportWidth();
	float GetViewportHeight();
	void SetViewportDimensions(float x, float y);
	void SetCursorVisible(bool isVisible);
private:
	static Window* s_Instance;
	GLFWwindow* window;
	float windowWidth, windowHeight;
	float viewportWidth, viewportHeight;
};

#endif