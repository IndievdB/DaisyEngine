#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include "Renderer/Window.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Input.hpp"
#include "Vendor/entt/entt.hpp"

#include "Demo.hpp"

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