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

RenderSystem::RenderSystem(std::shared_ptr<entt::registry> registry) : skybox ("Resources/PBR/Malibu_Overlook_3k.hdr")
{
	pbrSettings.Setup(skybox.environmentCubemap);
	clusteredSettings = new ClusteredSettings(registry, NUM_LIGHTS, 8, 8, 8, Vector2(-1, -1), Vector2(1, 1));
}

void RenderSystem::RenderAll(std::shared_ptr<entt::registry> registry)
{
	Matrix4x4 projection;
	Matrix4x4 view;
	Vector3 cameraPosition;

	registry->view<Transform, Camera>().each([&projection, &view, &cameraPosition](auto& transform, auto& camera)
	{
		projection = Matrix4x4::Perspective(camera.fov * kDegToRad, 800.0f / 600.0f, camera.nearPlane, camera.farPlane);

		cameraPosition = transform.position;

		Matrix3x3 rotationMatrix(transform.rotation);
		Vector3 forward = rotationMatrix * Vector3::forward;
		Vector3 up = rotationMatrix * Vector3::up;
		view = Matrix4x4::LookAt(transform.position, transform.position + forward, up);
	});

	clusteredSettings->Update(projection, view, cameraPosition);

	std::shared_ptr<DirectionalLight> directionalLight;
	registry->view<Transform, DirectionalLight>().each([&directionalLight](auto& transform, auto& light)
	{
		directionalLight = std::make_shared<DirectionalLight>(light);
	});

















	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// -----------------------

	 // 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	Matrix4x4 lightProjection, lightView;
	Matrix4x4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 50.0f;
	//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
	lightProjection = Matrix4x4::Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	Vector3 lightPos(-directionalLight->direction.x * 10.0f, -directionalLight->direction.y * 10.0f, -directionalLight->direction.z * 10.0f);
	lightView = Matrix4x4::LookAt(lightPos, Vector3::zero, Vector3::one);
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	std::shared_ptr<Shader> depthShader = ResourceManager::GetInstance()->GetShader("Resources/Clustered/shadowMappingDepth.shader");
	depthShader->Use();
	depthShader->SetMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	registry->view<Transform, MeshRenderer>().each([&depthShader](auto& transform, auto& meshRenderer)
	{
		Matrix4x4 model = Matrix4x4::Transformation(transform);
		depthShader->SetMatrix4x4("model", model);
		meshRenderer.mesh->Render(depthShader, 0.01f);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 800, 600);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	
	/*Mesh fullscreenQuad("Resources/PBR/upsideDownQuad.obj");
	Shader fullscreenShader("Resources/debugDepth.shader");
	fullscreenShader.Use();
	fullscreenShader.SetFloat("near_plane", near_plane);
	fullscreenShader.SetFloat("far_plane", far_plane);

	glActiveTexture(GL_TEXTURE0 + fullscreenShader.GetTextureUnit("depthMap"));
	glBindTexture(GL_TEXTURE_2D, depthMap);

	fullscreenQuad.Render();
	return;*/
















	registry->view<Transform, MeshRenderer>().each([projection, view, cameraPosition, directionalLight, lightSpaceMatrix, depthMap, this](auto& transform, auto& meshRenderer)
	{
		Matrix4x4 model = Matrix4x4::Transformation(transform);
		std::shared_ptr<Shader> shader = meshRenderer.material->GetShader();

		shader->Use();
		shader->SetMatrix4x4("projection", projection);
		shader->SetMatrix4x4("view", view);
		shader->SetMatrix4x4("model", model);
		shader->SetVector3("camPos", cameraPosition.x, cameraPosition.y, cameraPosition.z);
		shader->SetFloat("nearPlane", 1.0f);
		shader->SetFloat("farPlane", 4000.0f);
		shader->SetFloat("ambientLighting", 0.05f);

		pbrSettings.Bind(shader);

		if (directionalLight != nullptr)
		{
			shader->SetMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);
			shader->SetVector3("lightPos", -directionalLight->direction.x, -directionalLight->direction.y, -directionalLight->direction.z);
			shader->SetVector3("directionalLight.direction", directionalLight->direction.x, directionalLight->direction.y, directionalLight->direction.z);
			shader->SetVector3("directionalLight.color", directionalLight->color.x, directionalLight->color.y, directionalLight->color.z);

			glActiveTexture(GL_TEXTURE0 + shader->GetTextureUnit("shadowMap"));
			glBindTexture(GL_TEXTURE_2D, depthMap);
		}

		meshRenderer.material->Bind();
		meshRenderer.mesh->Render(shader, 0.01f);
	});

	skybox.Render(projection, view);

	registry->view<Transform, TextMesh>().each([](auto& transform, auto& textMesh)
	{
		TextMesh::Render(textMesh, transform.position.x, transform.position.y);
	});
}