#include "Skybox.hpp"
#include "../Core/Math/Mathf.hpp"
#include "../Core/Math/Vector3.hpp"
#include "Framebuffer.hpp"
#include "Window.hpp"
#include "../Core/ResourceManager.hpp"

Skybox::Skybox(std::string filePath)
{
	Shader equirectangularToCubemapShader("Resources/PBR/EquirectangularToCubemap.shader");

	Framebuffer offscreenFramebuffer;
	Renderbuffer offscreenDepthRenderbuffer(GL_DEPTH_COMPONENT24, 512, 512);
	offscreenFramebuffer.AttachRenderbuffer(offscreenDepthRenderbuffer, GL_DEPTH_ATTACHMENT);

	Texture hdrTexture(filePath);

	environmentCubemap = std::make_shared<Cubemap>(GL_LINEAR_MIPMAP_LINEAR, 512, 512);

	Matrix4x4 captureProjection = Matrix4x4::Perspective(90.0f * kDegToRad, 1.0f, 0.1f, 10.0f);
	Matrix4x4 captureViews[] =
	{
		Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
		Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
		Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
		Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
		Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
		Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
	};

	equirectangularToCubemapShader.Use();
	equirectangularToCubemapShader.SetMatrix4x4("projection", captureProjection);
	hdrTexture.Bind(equirectangularToCubemapShader.GetTextureUnit("equirectangularMap"));

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	offscreenFramebuffer.Bind();

	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.SetMatrix4x4("view", captureViews[i]);
		offscreenFramebuffer.AttachCubemapFace(*environmentCubemap, i, GL_COLOR_ATTACHMENT0, 0);
		Window::GetInstance()->Clear();

		ResourceManager::GetInstance()->GetMesh("Resources/PBR/InvertedCube.obj")->Render();
	}

	Framebuffer::Unbind();

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	environmentCubemap->GenerateMipMaps();

	Window::GetInstance()->ResetDimensions();
}

void Skybox::Render(const Matrix4x4& projection, const Matrix4x4& view)
{
	auto skyboxShader = ResourceManager::GetInstance()->GetShader("Resources/Engine/Shaders/Skybox.shader");
	skyboxShader->Use();
	skyboxShader->SetMatrix4x4("projection", projection);
	skyboxShader->SetMatrix4x4("view", view);
	environmentCubemap->Bind(skyboxShader->GetTextureUnit("environmentMap"));
	ResourceManager::GetInstance()->GetMesh("Resources/Engine/Meshes/InvertedCube.obj")->Render();
}