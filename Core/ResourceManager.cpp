#include "ResourceManager.hpp"

ResourceManager* ResourceManager::s_Instance;

ResourceManager::ResourceManager()
{
	ResourceManager::s_Instance = this;
}

ResourceManager* ResourceManager::GetInstance()
{
	return ResourceManager::s_Instance;
}

// Texture

void ResourceManager::LoadTexture(std::string path)
{
	if (textures.find(path) != textures.end())
		return;

	std::shared_ptr<Texture> resourcePointer = std::make_shared<Texture>(path);
	textures.emplace(path, resourcePointer);
}

std::shared_ptr<Texture> ResourceManager::GetTexture(std::string path)
{
	if (textures.find(path) == textures.end())
		LoadTexture(path);

	return std::shared_ptr<Texture>(textures.find(path)->second);
}

// Shader

void ResourceManager::LoadShader(std::string path)
{
	if (shaders.find(path) != shaders.end())
		return;

	std::shared_ptr<Shader> resourcePointer = std::make_shared<Shader>(path);
	shaders.emplace(path, resourcePointer);
}

std::shared_ptr<Shader> ResourceManager::GetShader(std::string path)
{
	if (shaders.find(path) == shaders.end())
		LoadShader(path);

	return std::shared_ptr<Shader>(shaders.find(path)->second);
}

// Material

void ResourceManager::LoadMaterial(std::string path)
{
	if (materials.find(path) != materials.end())
		return;

	std::shared_ptr<Material> resourcePointer = std::make_shared<Material>(path);
	materials.emplace(path, resourcePointer);
}

std::shared_ptr<Material> ResourceManager::GetMaterial(std::string path)
{
	if (materials.find(path) == materials.end())
		LoadMaterial(path);

	return std::shared_ptr<Material>(materials.find(path)->second);
}

// Mesh

void ResourceManager::LoadMesh(std::string path)
{
	if (meshes.find(path) != meshes.end())
		return;

	std::shared_ptr<Mesh> resourcePointer = std::make_shared<Mesh>(path);
	meshes.emplace(path, resourcePointer);
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(std::string path)
{
	if (meshes.find(path) == meshes.end())
		LoadMesh(path);

	return std::shared_ptr<Mesh>(meshes.find(path)->second);
}

// Fonts

void ResourceManager::LoadFont(std::string path)
{
	if (fonts.find(path) != fonts.end())
		return;

	std::shared_ptr<Font> resourcePointer = std::make_shared<Font>(path);
	fonts.emplace(path, resourcePointer);
}

std::shared_ptr<Font> ResourceManager::GetFont(std::string path)
{
	if (fonts.find(path) == fonts.end())
		LoadFont(path);

	return std::shared_ptr<Font>(fonts.find(path)->second);
}

std::string ResourceManager::ResourcePathFromMaterial(std::shared_ptr<Material> material)
{
	for (auto const& item : materials)
	{
		if (item.second == material)
			return item.first;
	}

	return "unknown";
}

std::string ResourceManager::ResourcePathFromMesh(std::shared_ptr<Mesh> mesh)
{
	for (auto const& item : meshes)
	{
		if (item.second == mesh)
			return item.first;
	}

	return "unknown";
}