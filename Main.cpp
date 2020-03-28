#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "entt/entt.hpp"
#include <glm/glm.hpp>

#include "Renderer/Window.hpp"
#include "Core/Input.hpp"
#include "Renderer/Camera.hpp"
#include "Core/ResourceManager.hpp"

#include "Renderer/RenderSystem.hpp"
#include "Demo.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Physics/cycloneMath.hpp"

int main()
{
	ResourceManager resourceManager;
	Input inputSystem;
	Window window("Daisy", 800, 600);
	entt::registry registry;
	PhysicsSystem physicsSystem;

	LoadScene(registry);

	float currentTime, deltaTime, previousTime;
	previousTime = currentTime = glfwGetTime();

	while (window.isOpen)
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - previousTime;
		previousTime = currentTime;

		inputSystem.Update();
		window.ProcessInput();
		window.Clear();

		RenderAll(registry);

		if (Input::GetKey(KeyCode::Space))
			physicsSystem.RunPhysics(registry);

		window.SwapBuffers();
	}
}