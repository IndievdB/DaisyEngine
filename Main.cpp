#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include "Renderer/Window.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Input.hpp"
#include "Vendor/entt/entt.hpp"
#include "Behaviour/LuaSystem.hpp"

//#include "Explosion.hpp"
#include "PBRDemo.hpp"

int main()
{
	ResourceManager resourceManager;
	Input inputSystem;
	Window window("Daisy", 800, 600);
	auto registry = std::make_shared<entt::registry>();
	PhysicsSystem physicsSystem;
	LuaSystem luaSystem(registry);
	RenderSystem renderSystem;

	LoadScene(registry);

	luaSystem.Intialize();

	while (window.isOpen)
	{
		inputSystem.Update();
		window.ProcessInput();
		window.Clear();

		luaSystem.Update();

		renderSystem.RenderAll(registry);

		physicsSystem.RunPhysics(registry);

		window.SwapBuffers();
	}
}