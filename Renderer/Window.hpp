#ifndef Window_hpp
#define Window_hpp

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
	float GetWidth();
	float GetHeight();
private:
	static Window* s_Instance;
	GLFWwindow* window;
	float width, height;
};

#endif