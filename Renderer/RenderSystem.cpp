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


RenderSystem::RenderSystem(std::shared_ptr<entt::registry> registry) : skybox ("Resources/PBR/Malibu_Overlook_3k.hdr"), registry(registry)
{
	pbrSettings.Setup(skybox.environmentCubemap);
	clusteredSettings = new ClusteredSettings(registry, NUM_LIGHTS);
	shadowSettings = new ShadowSettings(registry);
}

void RenderSystem::RenderAll(std::shared_ptr<Texture> renderTexture, std::shared_ptr<Renderbuffer> depthBuffer, std::shared_ptr<Transform> cameraTransform, std::shared_ptr<Camera> camera)
{
	Matrix4x4 projection = Matrix4x4::Perspective(camera->fov * kDegToRad, Window::GetInstance()->GetViewportWidth() / Window::GetInstance()->GetViewportHeight(), camera->nearPlane, camera->farPlane);

	Matrix3x3 rotationMatrix(cameraTransform->rotation);
	Vector3 forward = rotationMatrix * Vector3::forward;
	Vector3 up = rotationMatrix * Vector3::up;
	Matrix4x4 view = Matrix4x4::LookAt(cameraTransform->position, cameraTransform->position + forward, up);

	clusteredSettings->Update(projection, view, cameraTransform->position, camera->nearPlane, camera->farPlane);

	std::shared_ptr<DirectionalLight> directionalLight;
	registry->view<Transform, DirectionalLight>().each([&directionalLight](auto& transform, auto& light)
	{
		directionalLight = std::make_shared<DirectionalLight>(light);
	});

	shadowSettings->Update(camera, view);
	
	offscreenFramebuffer.Bind();
	offscreenFramebuffer.AttachRenderbuffer(*depthBuffer, GL_DEPTH_ATTACHMENT);
	offscreenFramebuffer.AttachTexture(*renderTexture, GL_COLOR_ATTACHMENT0);

	Window::GetInstance()->Clear();

	registry->view<Transform, MeshRenderer>().each([&projection, &view, &cameraTransform, &directionalLight, &camera, this](auto& transform, auto& meshRenderer)
		{
			Matrix4x4 model = Matrix4x4::Transformation(transform);
			std::shared_ptr<Shader> shader = meshRenderer.material->GetShader();

			shader->Use();
			shader->SetMatrix4x4("projection", projection);
			shader->SetMatrix4x4("view", view);
			shader->SetMatrix4x4("model", model);
			shader->SetVector3("camPos", cameraTransform->position.x, cameraTransform->position.y, cameraTransform->position.z);
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
}