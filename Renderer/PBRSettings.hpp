#pragma once

#include <memory>
#include "Cubemap.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

class PBRSettings
{
public:
	void Setup(std::shared_ptr<Cubemap> environmentCubemap);
	void Bind(std::shared_ptr<Shader> shader);
private:
	std::unique_ptr<Cubemap> irradianceCubemap;
	std::unique_ptr<Cubemap> prefilterCubemap;
	//std::unique_ptr<Cubemap> environmentCubemap;
	std::unique_ptr<Texture> brdfLUTTexture;
};