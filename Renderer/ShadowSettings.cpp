#include "ShadowSettings.hpp"
#include "../Core/ResourceManager.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "Framebuffer.hpp"
#include "MeshRenderer.hpp"
#include "Window.hpp"
#include "../Core/Transform.hpp"
#include "../Core/Math/Mathf.hpp"
#include <algorithm>
#include "Skybox.hpp"


ShadowSettings::ShadowSettings(std::shared_ptr<entt::registry> registry)
{
	this->registry = registry;

	shadowMap = new Texture(1024, 1024, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_NEAREST, GL_CLAMP_TO_BORDER);
	shadowMap->AddBorder(1, 1, 1, 1);


	//

	cube = new Mesh("Resources/sphere.obj");
	cubeShader = new Shader("Resources/cubecube.shader");
	skybox = new Skybox("Resources/PBR/Malibu_Overlook_3k.hdr");
	shadowCubeMap = std::make_unique<Cubemap>(GL_LINEAR, 1024, 1024);
}

void ShadowSettings::TempDirectionalLight(std::shared_ptr<Camera> camera, Matrix4x4& view)
{
	// 1. GET DIRECTIONAL LIGHT

	std::shared_ptr<DirectionalLight> directionalLight;
	std::shared_ptr<Transform> transform;
	registry->view<Transform, DirectionalLight>().each([&transform, &directionalLight](auto& trans, auto& light)
	{
		transform = std::make_shared<Transform>(trans);
		directionalLight = std::make_shared<DirectionalLight>(light);
	});

	if (directionalLight == nullptr)
		return;

	// 2. CREATE FRAMEBUFFER

	Framebuffer offscreenFramebuffer;
	offscreenFramebuffer.AttachTexture(*shadowMap, GL_DEPTH_ATTACHMENT);

	// 3. CALCULATE BOUNDS TO COVER FRUSTUM

	float FOV = camera->fov + 2.0f;

	Matrix4x4 viewInverse = Matrix4x4::AffineInverse(view);
	Matrix4x4 lightView = Matrix4x4::LookAt(Vector3::zero, directionalLight->direction, Vector3::one);

	float aspectRatio = (float)Window::GetInstance()->GetWidth() / (float)Window::GetInstance()->GetHeight();
	float tanHalfHFOV = tanf(kDegToRad * (FOV / 2.0f));
	float tanHalfVFOV = tanf(kDegToRad * ((FOV * aspectRatio) / 2.0f));

	float xn = camera->nearPlane * tanHalfHFOV;
	float xf = camera->farPlane * tanHalfHFOV;
	float yn = camera->nearPlane * tanHalfVFOV;
	float yf = camera->farPlane * tanHalfVFOV;

	Vector4 frustumCornersL[8];
	Vector4 frustumCorners[8] =
	{
		Vector4(xn, yn, -camera->nearPlane, 1.0f),
		Vector4(-xn, yn, -camera->nearPlane, 1.0f),
		Vector4(xn, -yn, -camera->nearPlane, 1.0f),
		Vector4(-xn, -yn, -camera->nearPlane, 1.0f),

		Vector4(xf, yf, -camera->farPlane, 1.0f),
		Vector4(-xf, yf, -camera->farPlane, 1.0f),
		Vector4(xf, -yf, -camera->farPlane, 1.0f),
		Vector4(-xf, -yf, -camera->farPlane, 1.0f)
	};

	float minX = INFINITY;
	float maxX = -INFINITY;
	float minY = INFINITY;
	float maxY = -INFINITY;
	float minZ = INFINITY;
	float maxZ = -INFINITY;

	for (int j = 0; j < 8; j++)
	{
		Vector4 vW = viewInverse * frustumCorners[j];

		frustumCornersL[j] = lightView * vW;

		minX = std::min(minX, frustumCornersL[j].x);
		maxX = std::max(maxX, frustumCornersL[j].x);
		minY = std::min(minY, frustumCornersL[j].y);
		maxY = std::max(maxY, frustumCornersL[j].y);
		minZ = std::min(minZ, -frustumCornersL[j].z);
		maxZ = std::max(maxZ, -frustumCornersL[j].z);
	}

	// 3. RENDER SCENE FROM LIGHTS PERSPECTIVE

	Matrix4x4 lightProjection = Matrix4x4::Orthographic(minX, maxX, minY, maxY, minZ - 50.0f, maxZ);

	lightSpaceMatrix = lightProjection * lightView;
	std::shared_ptr<Shader> depthShader = ResourceManager::GetInstance()->GetShader("Resources/Clustered/shadowMappingDepth.shader");
	depthShader->Use();
	depthShader->SetMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, 1024, 1024);
	offscreenFramebuffer.Bind();
	Window::GetInstance()->Clear();

	registry->view<Transform, MeshRenderer>().each([&depthShader](auto& transform, auto& meshRenderer)
	{
		Matrix4x4 model = Matrix4x4::Transformation(transform);
		depthShader->SetMatrix4x4("model", model);
		meshRenderer.mesh->Render(depthShader, 0.01f);
	});

	Framebuffer::Unbind();
	Window::GetInstance()->ResetDimensions();
	Window::GetInstance()->Clear();

	// 4. OPTIONALLY RENDER SHADOW TEXTURE TO SCREEN

	return;
	Mesh quad("Resources/PBR/FullscreenQuad.obj");
	Shader quadShader("Resources/texture.shader");
	quadShader.Use();
	shadowMap->Bind(quadShader.GetTextureUnit("mainTex"));
	quad.Render();
}

void ShadowSettings::TempPointLight(std::shared_ptr<Camera> camera, Matrix4x4& view)
{
	// 1. GET POINT LIGHT

	std::shared_ptr<PointLight> pointLight;
	std::shared_ptr<Transform> transform;
	registry->view<Transform, PointLight>().each([&transform, &pointLight](auto& trans, auto& light)
	{
		transform = std::make_shared<Transform>(trans);
		pointLight = std::make_shared<PointLight>(light);
	});

	if (pointLight == nullptr)
		return;

	// 2. CREATE FRAMEBUFFER

	//shadowCubeMap = std::make_unique<Cubemap>(512, 512);
	//shadowCubeMap = std::make_unique<Cubemap>(GL_LINEAR, 1024, 1024);


	Framebuffer offscreenFramebuffer;
	offscreenFramebuffer.Bind();

	Renderbuffer offscreenDepthRenderbuffer(GL_DEPTH_COMPONENT24, 1024, 1024);
	offscreenFramebuffer.AttachRenderbuffer(offscreenDepthRenderbuffer, GL_DEPTH_ATTACHMENT);

	// 3. RENDER SCENE FROM LIGHTS PERSPECTIVE

	Matrix4x4 captureProjection = Matrix4x4::Perspective(90.0f * kDegToRad, 1.0f, camera->nearPlane, camera->farPlane);
	Matrix4x4 captureViews[] =
	{
		Matrix4x4::LookAt(transform->position, transform->position + Vector3(1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
		Matrix4x4::LookAt(transform->position, transform->position + Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
		Matrix4x4::LookAt(transform->position, transform->position + Vector3(0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
		Matrix4x4::LookAt(transform->position, transform->position + Vector3(0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
		Matrix4x4::LookAt(transform->position, transform->position + Vector3(0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
		Matrix4x4::LookAt(transform->position, transform->position + Vector3(0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
	};

	glViewport(0, 0, 1024, 1024);
	Window::GetInstance()->Clear();

	for (unsigned int i = 0; i < 6; i++)
	{
		lightSpaceMatrix = captureProjection * captureViews[i];
		std::shared_ptr<Shader> depthShader = ResourceManager::GetInstance()->GetShader("Resources/Clustered/pointShadowDepth.shader");
		depthShader->Use();
		depthShader->SetMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);
		depthShader->SetVector3("lightPos", transform->position.x, transform->position.y, transform->position.z);
		depthShader->SetFloat("far_plane", camera->farPlane);

		offscreenFramebuffer.AttachCubemapFace(*shadowCubeMap, i, GL_COLOR_ATTACHMENT0, 0);
		Window::GetInstance()->Clear();
		//offscreenFramebuffer.AttachCubemapFace(*shadowCubeMap, i, GL_DEPTH_ATTACHMENT, 0);
		//Window::GetInstance()->Clear();

		registry->view<Transform, MeshRenderer>().each([&depthShader](auto& transform, auto& meshRenderer)
		{
			Matrix4x4 model = Matrix4x4::Transformation(transform);
			depthShader->SetMatrix4x4("model", model);
			meshRenderer.mesh->Render(depthShader, 0.01f);
		});
	}

	Framebuffer::Unbind();
	Window::GetInstance()->ResetDimensions();
	Window::GetInstance()->Clear();

	// 4. OPTIONALLY RENDER SHADOW TEXTURE TO SCREEN

	Matrix4x4 model = Matrix4x4::Transformation(Vector3(0, 8, 0), Quaternion::identity);
	Matrix4x4 projection = Matrix4x4::Perspective(camera->fov * kDegToRad, 800.0f / 600.0f, camera->nearPlane, camera->farPlane);
	
	cubeShader->Use();
	cubeShader->SetMatrix4x4("model", model);
	cubeShader->SetMatrix4x4("view", view);
	cubeShader->SetMatrix4x4("projection", projection);
	//skybox->environmentCubemap->Bind(cubeShader->GetTextureUnit("cubemap"));
	shadowCubeMap->Bind(cubeShader->GetTextureUnit("cubemap"));

	cube->Render();
}

void ShadowSettings::Update(std::shared_ptr<Camera> camera, Matrix4x4& view)
{
	TempDirectionalLight(camera, view);
	//TempPointLight(camera, view);
}

void ShadowSettings::Bind(std::shared_ptr<Shader> shader)
{
	shader->SetMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);
	shadowMap->Bind(shader->GetTextureUnit("shadowMap"));
}