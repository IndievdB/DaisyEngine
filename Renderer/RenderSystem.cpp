#include "RenderSystem.hpp"

#include "../Core/Transform.hpp"
#include "../Core/Math/Matrix3x3.hpp"
#include "../Core/Math/Matrix4x4.hpp"
#include "../Core/Math/Quaternion.hpp"
#include "../Core/Math/Vector3.hpp"
#include "../Core/Math/Mathf.hpp"
#include "Camera.hpp"
#include "TextMesh.hpp"
#include "MeshRenderer.hpp"
#include "Material.hpp"
#include "Framebuffer.hpp"
#include "Window.hpp"
#include "DirectionalLight.hpp"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_glfw.h"
#include "../Vendor/imgui/imgui_impl_opengl3.h"

RenderSystem::RenderSystem(std::shared_ptr<entt::registry> registry) : skybox ("Resources/PBR/Malibu_Overlook_3k.hdr")
{
	pbrSettings.Setup(skybox.environmentCubemap);
	clusteredSettings = new ClusteredSettings(registry, NUM_LIGHTS);
	shadowSettings = new ShadowSettings(registry);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Window::GetInstance()->GetGLFWWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 150");
}

RenderSystem::~RenderSystem()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void RenderSystem::RenderAll(std::shared_ptr<entt::registry> registry)
{
	Matrix4x4 projection;
	Matrix4x4 view;
	Vector3 cameraPosition;
	std::shared_ptr<Camera> camera;

	registry->view<Transform, Camera>().each([&projection, &view, &cameraPosition, &camera](auto& transform, auto& cam)
	{
		camera = std::make_shared<Camera>(cam);

		projection = Matrix4x4::Perspective(cam.fov * kDegToRad, 800.0f / 600.0f, cam.nearPlane, cam.farPlane);

		cameraPosition = transform.position;

		Matrix3x3 rotationMatrix(transform.rotation);
		Vector3 forward = rotationMatrix * Vector3::forward;
		Vector3 up = rotationMatrix * Vector3::up;
		view = Matrix4x4::LookAt(transform.position, transform.position + forward, up);
	});

	clusteredSettings->Update(projection, view, cameraPosition, camera->nearPlane, camera->farPlane);

	std::shared_ptr<DirectionalLight> directionalLight;
	registry->view<Transform, DirectionalLight>().each([&directionalLight](auto& transform, auto& light)
	{
		directionalLight = std::make_shared<DirectionalLight>(light);
	});

	shadowSettings->Update(camera, view);


	
	registry->view<Transform, MeshRenderer>().each([&projection, &view, &cameraPosition, &directionalLight, &camera, this](auto& transform, auto& meshRenderer)
	{
		Matrix4x4 model = Matrix4x4::Transformation(transform);
		std::shared_ptr<Shader> shader = meshRenderer.material->GetShader();

		shader->Use();
		shader->SetMatrix4x4("projection", projection);
		shader->SetMatrix4x4("view", view);
		shader->SetMatrix4x4("model", model);
		shader->SetVector3("camPos", cameraPosition.x, cameraPosition.y, cameraPosition.z);
		shader->SetFloat("nearPlane", camera->nearPlane);
		shader->SetFloat("farPlane", camera->farPlane);
		shader->SetFloat("screenWidth", Window::GetInstance()->GetWidth());
		shader->SetFloat("screenHeight", Window::GetInstance()->GetHeight());
		shader->SetFloat("ambientLighting", 0.05f);

		pbrSettings.Bind(shader);

		if (directionalLight != nullptr)
		{
			shader->SetVector3("directionalLight.direction", directionalLight->direction.x, directionalLight->direction.y, directionalLight->direction.z);
			shader->SetVector3("directionalLight.color", directionalLight->color.x, directionalLight->color.y, directionalLight->color.z);
			shadowSettings->Bind(shader);
		}

		shadowSettings->shadowCubeMap->Bind(shader->GetTextureUnit("pointShadowMap"));

		meshRenderer.material->Bind();
		meshRenderer.mesh->Render(shader, 0.01f);
	});
	


	skybox.Render(projection, view);

	registry->view<Transform, TextMesh>().each([](auto& transform, auto& textMesh)
	{
		TextMesh::Render(textMesh, transform.position.x, transform.position.y);
	});


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiIO io = ImGui::GetIO();

	ImGui::Begin("Profiling");
	ImGui::Text("%f ms\n", io.DeltaTime * 1000.0f);
	ImGui::Text("%f fps\n", io.Framerate);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}