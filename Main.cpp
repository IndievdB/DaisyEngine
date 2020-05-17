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
//#include "Explosion.hpp"
//#include "Animation.hpp"
//#include "Shadows.hpp"

int main()
{
	ResourceManager resourceManager;
	Input inputSystem;
	Window window("Daisy", 1200, 700);
	auto registry = std::make_shared<entt::registry>();
	
	//LoadScene(registry);

	std::shared_ptr <PhysicsSystem> physicsSystem = std::make_shared<PhysicsSystem>();
	std::shared_ptr <LuaSystem> luaSystem = std::make_shared<LuaSystem>(registry);
	std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(registry);
	//luaSystem.Intialize();
	Editor editor(registry, renderSystem, physicsSystem, luaSystem);

	while (window.isOpen)
	{
		inputSystem.Update();
		window.ProcessInput();
		window.Clear();

		//luaSystem.Update();
		//renderSystem->RenderAll(registry);
		//physicsSystem.RunPhysics(registry);
		editor.Update();

		window.SwapBuffers();
	}
}