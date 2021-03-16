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
//#include "SimpleMove.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

int main()
{
	glm::eulerAngles()

	Quaternion TJS_1(0.696575, 0.447218, 0.389621, -0.394079);
	Vector3 TJS_2(1.2698299, 0.1929901, -1.1720636);
	Quaternion TJS_3(0.6992641, 0.4489445, 0.3911251, -0.3956004);

	std::cout << "T XYZ 1) " << TJS_1 << std::endl;
	std::cout << "T XYZ 2) " << TJS_2 << std::endl;
	std::cout << "T XYZ 3) " << TJS_3 << std::endl;
	std::cout << std::endl;

	Quaternion D_1(0.696575, 0.447218, 0.389621, -0.394079);
	Vector3 D_2 = D_1.GetEulerAnglesXYZ();
	Quaternion D_3(D_2.x, D_2.y, D_2.z);

	std::cout << "D XYZ 1) " << D_1 << std::endl;
	std::cout << "D XYZ 2) " << D_2 << std::endl;
	std::cout << "D XYZ 3) " << D_3 << std::endl;
	std::cout << std::endl;

	Quaternion TJS_4(0.696575, 0.447218, 0.389621, -0.394079);
	Vector3 TJS_5(0.8304378, 1.1248543, -0.4876014);
	Quaternion TJS_6(0.6992641, 0.4489445, 0.3911251, -0.3956003);

	std::cout << "T ZYX 1) " << TJS_4 << std::endl;
	std::cout << "T ZYX 2) " << TJS_5 << std::endl;
	std::cout << "T ZYX 3) " << TJS_6 << std::endl;
	std::cout << std::endl;

	Quaternion D_4(0.696575, 0.447218, 0.389621, -0.394079);
	Vector3 D_5 = D_1.GetEulerAnglesZYX();
	Quaternion D_6; D_6.SetZYX(D_5.x, D_5.y, D_5.z);

	std::cout << "D ZYX 1) " << D_4 << std::endl;
	std::cout << "D ZYX 2) " << D_5 << std::endl;
	std::cout << "D ZYX 3) " << D_6 << std::endl;
	std::cout << std::endl;

	/*ResourceManager resourceManager;
	Input inputSystem;
	Window window("Daisy Engine", 1200, 700);
	auto registry = std::make_shared<entt::registry>();
	
	LoadScene(registry);

	std::shared_ptr<PhysicsSystem> physicsSystem = std::make_shared<PhysicsSystem>();
	std::shared_ptr<LuaSystem> luaSystem = std::make_shared<LuaSystem>(registry);
	std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(registry);
	std::shared_ptr<Editor> editor = std::make_shared<Editor>(registry, renderSystem, physicsSystem, luaSystem);
	editor->AddWindows();

	while (window.isOpen)
	{
		inputSystem.Update();
		window.ProcessInput();
		window.Clear();

		editor->Update();

		window.SwapBuffers();
	}*/
}