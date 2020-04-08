#include "PBRSettings.hpp"

#include "Framebuffer.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "../Core/ResourceManager.hpp"
#include "Window.hpp"
#include "../Core/Math/Mathf.hpp"

void PBRSettings::Setup(std::shared_ptr<Cubemap> environmentCubemap)
{
	Shader equirectangularToCubemapShader("Resources/PBR/EquirectangularToCubemap.shader");
	Shader irradianceShader("Resources/PBR/Irradiance.shader");
	Shader prefilterShader("Resources/PBR/Prefilter.shader");
	Shader brdfShader("Resources/PBR/BRDF.shader");
	Shader skyboxShader("Resources/PBR/Skybox.shader");

	Mesh fullscreenQuad("Resources/PBR/upsideDownQuad.obj");

	Framebuffer offscreenFramebuffer;
	Renderbuffer offscreenDepthRenderbuffer(GL_DEPTH_COMPONENT24, 512, 512);
	offscreenFramebuffer.AttachRenderbuffer(offscreenDepthRenderbuffer, GL_DEPTH_ATTACHMENT);

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

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------

	irradianceCubemap = std::make_unique<Cubemap>(GL_LINEAR, 32, 32);

	offscreenFramebuffer.Bind();
	offscreenDepthRenderbuffer.Reformat(GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	irradianceShader.Use();
	irradianceShader.SetMatrix4x4("projection", captureProjection);
	environmentCubemap->Bind(irradianceShader.GetTextureUnit("environmentMap"));

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	offscreenFramebuffer.Bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.SetMatrix4x4("view", captureViews[i]);
		offscreenFramebuffer.AttachCubemapFace(*irradianceCubemap, i, GL_COLOR_ATTACHMENT0, 0);
		Window::GetInstance()->Clear();

		ResourceManager::GetInstance()->GetMesh("Resources/PBR/InvertedCube.obj")->Render();
	}
	Framebuffer::Unbind();

	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	// --------------------------------------------------------------------------------

	prefilterCubemap = std::make_unique<Cubemap>(GL_LINEAR_MIPMAP_LINEAR, 128, 128);
	prefilterCubemap->GenerateMipMaps();

	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	prefilterShader.Use();
	prefilterShader.SetMatrix4x4("projection", captureProjection);
	environmentCubemap->Bind(prefilterShader.GetTextureUnit("environmentMap"));

	offscreenFramebuffer.Bind();
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		offscreenDepthRenderbuffer.Reformat(GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.SetMatrix4x4("view", captureViews[i]);
			offscreenFramebuffer.AttachCubemapFace(*prefilterCubemap, i, GL_COLOR_ATTACHMENT0, mip);

			Window::GetInstance()->Clear();
			ResourceManager::GetInstance()->GetMesh("Resources/PBR/InvertedCube.obj")->Render();
		}
	}
	Framebuffer::Unbind();

	// pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------

	brdfLUTTexture = std::make_unique<Texture>(512, 512, GL_RG16F, GL_RG);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	offscreenFramebuffer.Bind();
	offscreenDepthRenderbuffer.Reformat(GL_DEPTH_COMPONENT24, 512, 512);

	offscreenFramebuffer.AttachTexture(*brdfLUTTexture, GL_COLOR_ATTACHMENT0);

	glViewport(0, 0, 512, 512);
	brdfShader.Use();
	Window::GetInstance()->Clear();
	fullscreenQuad.Render();

	Framebuffer::Unbind();

	Window::GetInstance()->ResetDimensions();
}

void PBRSettings::Bind(std::shared_ptr<Shader> shader)
{
	irradianceCubemap->Bind(shader->GetTextureUnit("irradianceMap"));
	prefilterCubemap->Bind(shader->GetTextureUnit("prefilterMap"));
	brdfLUTTexture->Bind(shader->GetTextureUnit("brdfLUT"));
}