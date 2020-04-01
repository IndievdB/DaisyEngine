#pragma once

#include "../Vendor/entt/entt.hpp"
#include "MeshRenderer.hpp"
#include "../Core/Transform.hpp"
#include "../Core/Math/Matrix3x3.hpp"
#include "../Core/Math/Matrix4x4.hpp"
#include "../Core/Math/Quaternion.hpp"
#include "../Core/Math/Mathf.hpp"
#include "Camera.hpp"

void RenderAll(std::shared_ptr<entt::registry> registry)
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
		meshRenderer.material->Bind();
		meshRenderer.mesh->Render();
	});
}