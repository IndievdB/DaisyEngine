#include "Window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Core/Input.hpp"

#include <iostream>

void FramebufferSizeCallback(GLFWwindow* p_window, int width, int height);
Window* Window::s_Instance;

Window::Window(const char* title, float width, float height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	this->width = width;
	this->height = height;

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	Input::SetActiveWindow(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	isOpen = true;

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Window::s_Instance = this;
}

float Window::GetWidth()
{
	return this->width;
}

float Window::GetHeight()
{
	return this->height;
}

void Window::ResetDimensions()
{
	int frameBufferWidth, frameBufferHeight;
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	glViewport(0, 0, frameBufferWidth, frameBufferHeight);
}

Window* Window::GetInstance()
{
	return Window::s_Instance;
}

GLFWwindow* Window::GetGLFWWindow()
{
	return window;
}

Window::~Window()
{
	glfwDestroyWindow(window);
}

void Window::ProcessInput()
{
	if (Input::GetKey(KeyCode::Escape) || Input::GetWindowCloseButton())
	{
		glfwSetWindowShouldClose(window, true);
		isOpen = false;
	}
}

void Window::Clear()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}


void FramebufferSizeCallback(GLFWwindow* p_window, int width, int height)
{
	glViewport(0, 0, width, height);
}