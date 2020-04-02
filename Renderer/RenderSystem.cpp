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

void RenderSystem::RenderAll(std::shared_ptr<entt::registry> registry)
{
	Matrix4x4 projection;
	Matrix4x4 view;

	registry->view<Transform, Camera>().each([&projection, &view](auto& transform, auto& camera)
	{
		projection = Matrix4x4::Perspective(camera.fov * kDegToRad, 800.0f / 600.0f, camera.nearPlane, camera.farPlane);

		Matrix3x3 rotationMatrix(transform.rotation);
		Vector3 forward = rotationMatrix * Vector3::forward;
		Vector3 up = rotationMatrix * Vector3::up;
		view = Matrix4x4::LookAt(transform.position, transform.position + forward, up);
	});

	registry->view<Transform, MeshRenderer>().each([projection, view](auto& transform, auto& meshRenderer)
	{
		Matrix4x4 model = Matrix4x4::Transformation(transform);

		meshRenderer.material->GetShader()->Use();
		meshRenderer.material->GetShader()->SetMatrix4x4("projection", projection);
		meshRenderer.material->GetShader()->SetMatrix4x4("view", view);
		meshRenderer.material->GetShader()->SetMatrix4x4("model", model);

		meshRenderer.material->GetShader()->SetVector3("dirLight.direction", 0.5f, -1, 0);
		meshRenderer.material->GetShader()->SetVector3("dirLight.color", 0.2, 0.2, 0.2);

		meshRenderer.material->GetShader()->SetVector3("spotLight.position", 0, 10, 0);
		meshRenderer.material->GetShader()->SetVector3("spotLight.direction", 0, -1, 0);
		meshRenderer.material->GetShader()->SetVector3("spotLight.color", 1, 1, 1);
		meshRenderer.material->GetShader()->SetFloat("spotLight.constant", 1.0f);
		meshRenderer.material->GetShader()->SetFloat("spotLight.linear", 0.09f);
		meshRenderer.material->GetShader()->SetFloat("spotLight.quadratic", 0.032f);
		meshRenderer.material->GetShader()->SetFloat("spotLight.cutOff", cosf(25.0f * kDegToRad));
		meshRenderer.material->GetShader()->SetFloat("spotLight.outerCutOff", cosf(30.0f * kDegToRad));

		meshRenderer.material->GetShader()->SetVector3("pointLights[0].position", 1.7f, 0.2f, 12.0f);
		meshRenderer.material->GetShader()->SetVector3("pointLights[0].color", 1, 0, 0);
		meshRenderer.material->GetShader()->SetFloat("pointLights[0].constant", 1.0f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[0].linear", 0.09f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[0].quadratic", 0.032f);

		meshRenderer.material->GetShader()->SetVector3("pointLights[1].position", 12.3f, 0.2f, -14.0f);
		meshRenderer.material->GetShader()->SetVector3("pointLights[1].color", 0, 1, 0);
		meshRenderer.material->GetShader()->SetFloat("pointLights[1].constant", 1.0f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[1].linear", 0.09f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[1].quadratic", 0.032f);

		meshRenderer.material->GetShader()->SetVector3("pointLights[2].position", -4.0f, 0.2f, -4.0f);
		meshRenderer.material->GetShader()->SetVector3("pointLights[2].color", 0, 0, 1);
		meshRenderer.material->GetShader()->SetFloat("pointLights[2].constant", 1.0f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[2].linear", 0.09f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[2].quadratic", 0.032f);

		meshRenderer.material->GetShader()->SetVector3("pointLights[3].position", 0.0f, 0.2f, -1.0f);
		meshRenderer.material->GetShader()->SetVector3("pointLights[3].color", 1, 0, 1);
		meshRenderer.material->GetShader()->SetFloat("pointLights[3].constant", 1.0f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[3].linear", 0.09f);
		meshRenderer.material->GetShader()->SetFloat("pointLights[3].quadratic", 0.032f);

		meshRenderer.material->Bind();
		meshRenderer.mesh->Render();
	});

	/*
		// point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032);
	*/


	registry->view<Transform, TextMesh>().each([](auto& transform, auto& textMesh)
	{
		TextMesh::Render(textMesh, transform.position.x, transform.position.y);
	});
}