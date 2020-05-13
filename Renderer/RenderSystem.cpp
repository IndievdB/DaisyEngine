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
#include "Window.hpp"
#include "DirectionalLight.hpp"


RenderSystem::RenderSystem(std::shared_ptr<entt::registry> registry) : skybox ("Resources/PBR/Malibu_Overlook_3k.hdr")
{
	pbrSettings.Setup(skybox.environmentCubemap);
	clusteredSettings = new ClusteredSettings(registry, NUM_LIGHTS);
	shadowSettings = new ShadowSettings(registry);

	offscreenTexture = std::make_shared<Texture>(Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight(), GL_RGB32F, GL_RGB);
	offscreenFramebuffer.AttachTexture(*offscreenTexture, GL_COLOR_ATTACHMENT0);
	offscreenDepthRenderbuffer = std::make_unique<Renderbuffer>(GL_DEPTH_COMPONENT24, Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight());
	offscreenFramebuffer.AttachRenderbuffer(*offscreenDepthRenderbuffer, GL_DEPTH_ATTACHMENT);
	Framebuffer::Unbind();
}

std::shared_ptr<Texture> RenderSystem::GetOffscreenTexture()
{
	return offscreenTexture;
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

		projection = Matrix4x4::Perspective(cam.fov * kDegToRad, Window::GetInstance()->GetViewportWidth() / Window::GetInstance()->GetViewportHeight(), cam.nearPlane, cam.farPlane);

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


	
	
	offscreenFramebuffer.Bind();
	offscreenDepthRenderbuffer->Reformat(GL_DEPTH_COMPONENT24, Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight());
	offscreenTexture->Reformat(Window::GetInstance()->GetViewportWidth(), Window::GetInstance()->GetViewportHeight(), GL_RGB32F, GL_RGB);
	
	Window::GetInstance()->Clear();

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
		shader->SetFloat("screenWidth", Window::GetInstance()->GetViewportWidth());
		shader->SetFloat("screenHeight", Window::GetInstance()->GetViewportHeight());
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

	Framebuffer::Unbind();

	/*Mesh quad("Resources/PBR/FullscreenQuad.obj");
	Shader quadShader("Resources/texture.shader");
	quadShader.Use();
	offscreenTexture->Bind(quadShader.GetTextureUnit("mainTex"));
	quad.Render();*/
}