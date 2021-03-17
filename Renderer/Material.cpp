#include "Material.hpp"
#include "../Core/ResourceManager.hpp"
#include "../Vendor/nlohmann/json.hpp"
#include <fstream>

Material::Material(std::string filePath)
{
	std::ifstream fileStream(filePath);
	nlohmann::json json;
	fileStream >> json;
	
	shader = ResourceManager::GetInstance()->GetShader(json["shader"]);

	for (auto const& entry : json["textures"].items())
		SetTexture(entry.key(), entry.value());

	for (auto const& entry : json["floats"].items())
		SetFloat(entry.key(), entry.value());
}

void Material::SetTexture(std::string name, std::string value)
{
	auto texturePointer = ResourceManager::GetInstance()->GetTexture(value);
	MaterialTextureProperty property(name, texturePointer);
	textureProperties.push_back(property);
}

void Material::SetFloat(std::string name, float value)
{
	MaterialFloatProperty property(name, value);
	floatProperties.push_back(property);
}

std::shared_ptr<Shader> Material::GetShader()
{
	return shader;
}

void Material::Bind()
{
	for (int i = 0; i < textureProperties.size(); i++)
	{
		auto texture = textureProperties[i].value;
		texture->Bind(shader->GetTextureUnit(textureProperties[i].name));
	}

	for (int i = 0; i < floatProperties.size(); i++)
	{
		shader->SetFloat(floatProperties[i].name, floatProperties[i].value);
	}
}