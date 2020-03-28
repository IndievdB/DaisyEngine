#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include "Renderer/Window.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Input.hpp"
#include "Vendor/entt/entt.hpp"

#include "Demo.hpp"
#include "Behaviour/LuaBehaviour.hpp"

int main()
{
	ResourceManager resourceManager;
	Input inputSystem;
	Window window("Daisy", 800, 600);
	entt::registry registry;
	PhysicsSystem physicsSystem;

	LoadScene(registry);

	LuaBehaviour testBehaviour("Resources/testing.lua");
	testBehaviour.Update();

	/*
	while (window.isOpen)
	{
		inputSystem.Update();
		window.ProcessInput();
		window.Clear();

		RenderAll(registry);

		if (Input::GetKey(KeyCode::Space))
			physicsSystem.RunPhysics(registry);

		window.SwapBuffers();
	}*/
}