#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include "Texture.hpp"
#include "Shader.hpp"

struct MaterialTextureProperty
{
	std::string name;
	std::shared_ptr<Texture> value;

	MaterialTextureProperty(std::string n, std::shared_ptr<Texture> v) : name(n), value(v) {};
};

struct MaterialFloatProperty
{
	std::string name;
	float value;

	MaterialFloatProperty(std::string n, float v) : name(n), value(v) {};
};

class Material
{
public:
	Material(std::string shaderPath);
	inline Material(std::shared_ptr<Shader> s) : shader(s) {};
	void SetTexture(std::string name, std::string texture);
	void SetFloat(std::string name, float value);
	std::shared_ptr<Shader> GetShader();
	void Bind();
private:
	std::shared_ptr<Shader> shader;
	std::vector<MaterialTextureProperty> textureProperties;
	std::vector<MaterialFloatProperty> floatProperties;
};