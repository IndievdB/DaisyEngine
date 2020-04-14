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

	registry->view<Transform, MeshRenderer>().each([projection, view, cameraPosition, this](auto& transform, auto& meshRenderer)
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

		//shader->SetVector3("lightPositions[0]", 0, 3, 0);
		//shader->SetVector3("lightColors[0]", 100, 0, 0);

		pbrSettings.Bind(shader);

		meshRenderer.material->Bind();
		meshRenderer.mesh->Render();
	});

	skybox.Render(projection, view);

	registry->view<Transform, TextMesh>().each([](auto& transform, auto& textMesh)
	{
		TextMesh::Render(textMesh, transform.position.x, transform.position.y);
	});
}