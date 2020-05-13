#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include "Renderer/Window.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Input.hpp"
#include "Vendor/entt/entt.hpp"
#include "Behaviour/LuaSystem.hpp"
#include "Editor/Editor.hpp"

//#include "PBRDemo.hpp"
//#include "Clustered.hpp"
#include "Explosion.hpp"
//#include "Animation.hpp"
//#include "Shadows.hpp"


int main()
{
	ResourceManager resourceManager;
	Input inputSystem;
	Window window("Daisy", 800, 600);
	auto registry = std::make_shared<entt::registry>();
	
	LoadScene(registry);

	PhysicsSystem physicsSystem;
	LuaSystem luaSystem(registry);
	std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(registry);
	luaSystem.Intialize();
	Editor editor(registry, renderSystem);

	while (window.isOpen)
	{
		inputSystem.Update();
		window.ProcessInput();
		window.Clear();

		luaSystem.Update();
		renderSystem->RenderAll(registry);
		physicsSystem.RunPhysics(registry);
		editor.Update();

		window.SwapBuffers();
	}
}