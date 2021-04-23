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
	shadowCubeMap = std::make_unique<Cubemap>(GL_LINEAR, 256, 256);
	cube = new Mesh("Resources/sphere.obj");
	cubeShader = new Shader("Resources/cubecube.shader");
	shadowCubeMapArray = std::make_unique<CubemapArray>(256);

	/*
	shadowMap = new Texture(1024, 1024, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_NEAREST, GL_CLAMP_TO_BORDER);
	shadowMap->AddBorder(1, 1, 1, 1);

	cube = new Mesh("Resources/sphere.obj");
	cubeShader = new Shader("Resources/cubecube.shader");
	skybox = new Skybox("Resources/PBR/Malibu_Overlook_3k.hdr");
	*/
}

/*void ShadowSettings::TempDirectionalLight(std::shared_ptr<Camera> camera, Matrix4x4& view)
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

	float aspectRatio = (float)Window::GetInstance()->GetViewportWidth() / (float)Window::GetInstance()->GetViewportHeight();
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
}*/

void ShadowSettings::RenderPointLightShadows(std::shared_ptr<Camera> camera, Matrix4x4& view)
{
	Framebuffer offscreenFramebuffer;

	int pointLightIndex = 0;
	registry->view<Transform, PointLight>().each([&offscreenFramebuffer, &pointLightIndex, this](auto& trans, auto& light)
	{
		if (!light.castsShadows)
		{
			pointLightIndex++;
			return;
		}

		GLenum err;

		Matrix4x4 captureProjection = Matrix4x4::Perspective(90.0f * kDegToRad, 1.0f, 0.01f, light.radius);
		Matrix4x4 captureViews[] =
		{
			Matrix4x4::LookAt(trans.position, trans.position + Vector3(1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
			Matrix4x4::LookAt(trans.position, trans.position + Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
			Matrix4x4::LookAt(trans.position, trans.position + Vector3(0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
			Matrix4x4::LookAt(trans.position, trans.position + Vector3(0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
			Matrix4x4::LookAt(trans.position, trans.position + Vector3(0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
			Matrix4x4::LookAt(trans.position, trans.position + Vector3(0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
		};

		glViewport(0, 0, 256, 256);
		Window::GetInstance()->Clear();

		for (unsigned int i = 0; i < 6; i++)
		{
			Matrix4x4 pointMatrix = captureProjection * captureViews[i];
			std::shared_ptr<Shader> depthShader = ResourceManager::GetInstance()->GetShader("Resources/Engine/Shaders/Shadows/PointShadowDepth.shader");
			depthShader->Use();
			depthShader->SetMatrix4x4("lightSpaceMatrix", pointMatrix);
			depthShader->SetVector3("lightPos", trans.position.x, trans.position.y, trans.position.z);
			depthShader->SetFloat("lightRadius", light.radius);

			offscreenFramebuffer.AttachCubemapArrayFace(*shadowCubeMapArray, pointLightIndex, i, GL_DEPTH_ATTACHMENT, 0);
			offscreenFramebuffer.Bind();
		
			Window::GetInstance()->Clear();
			registry->view<Transform, MeshRenderer>().each([&depthShader](auto& transform, auto& meshRenderer)
			{
				if (!meshRenderer.castsShadows)
					return;

				Matrix4x4 model = Matrix4x4::Transformation(transform);
				depthShader->SetMatrix4x4("model", model);
				meshRenderer.mesh->Render(depthShader, 0.01f);
			});
		}

		Framebuffer::Unbind();
		Window::GetInstance()->ResetDimensions();
		Window::GetInstance()->Clear();

		pointLightIndex++;
	});
}

void ShadowSettings::Update(std::shared_ptr<Camera> camera, Matrix4x4& view)
{
	//TempDirectionalLight(camera, view);
	//TempPointLight(camera, view);
	RenderPointLightShadows(camera, view);
}

void ShadowSettings::Bind(std::shared_ptr<Shader> shader)
{
	//shader->SetMatrix4x4("lightSpaceMatrix", lightSpaceMatrix);
	//shadowMap->Bind(shader->GetTextureUnit("shadowMap"));
}