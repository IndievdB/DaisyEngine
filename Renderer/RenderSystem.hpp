#pragma once

#include "../Vendor/entt/entt.hpp"
#include "MeshRenderer.hpp"
#include "../Core/Transform.hpp"
#include "Camera.hpp"
#include "../Physics/cycloneMath.hpp"

void RenderAll(entt::registry& registry)
{
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
	glm::mat4 view;

	registry.view<Transform, Camera>().each([&view](auto& transform, auto& camera)
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		glm::mat4 rotate = glm::mat4_cast(transform.rotation);
		matrix = matrix * rotate;

		glm::vec3 forward = TransformVector(matrix, glm::vec3(0, 0, 1));
		glm::vec3 up = TransformVector(matrix, glm::vec3(0, 1, 0));

		view = glm::lookAt(transform.position, transform.position + forward, up);
	});
	
	registry.view<Transform, MeshRenderer>().each([projection, view](auto& transform, auto& meshRenderer)
	{
		glm::mat4 model = Transform::ToMatrixGLM(transform);

		meshRenderer.material->GetShader()->Use();
		meshRenderer.material->GetShader()->SetMatrix4x4("projection", projection);
		meshRenderer.material->GetShader()->SetMatrix4x4("view", view);
		meshRenderer.material->GetShader()->SetMatrix4x4("model", model);
		meshRenderer.material->Bind();
		meshRenderer.mesh->Render();
	});
}