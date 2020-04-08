#pragma once

#include <memory>
#include <string>
#include "Cubemap.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

class Skybox
{
public:
	Skybox(std::string filePath);
	void Render(const Matrix4x4& projection, const Matrix4x4& view);
	std::shared_ptr<Cubemap> environmentCubemap;
};