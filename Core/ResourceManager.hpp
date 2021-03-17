#pragma once

#include "../Renderer/Material.hpp"
#include "../Renderer/Texture.hpp"
#include "../Renderer/Shader.hpp"
#include "../Renderer/Mesh.hpp"
#include "../Renderer/Font.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <memory>

class ResourceManager
{
public:
	ResourceManager();
	static ResourceManager* GetInstance();

	void LoadTexture(std::string path);
	void LoadShader(std::string path);
	void LoadMaterial(std::string path);
	void LoadMesh(std::string path);
	void LoadFont(std::string path);

	std::shared_ptr<Texture> GetTexture(std::string path);
	std::shared_ptr<Shader> GetShader(std::string path);
	std::shared_ptr<Material> GetMaterial(std::string path);
	std::shared_ptr<Mesh> GetMesh(std::string path);
	std::shared_ptr<Font> GetFont(std::string path);

	std::string ResourcePathFromMaterial(std::shared_ptr<Material> material);
	std::string ResourcePathFromMesh(std::shared_ptr<Mesh> mesh);

private:
	static ResourceManager* s_Instance;
	std::map<std::string, std::shared_ptr<Texture>> textures;
	std::map<std::string, std::shared_ptr<Shader>> shaders;
	std::map<std::string, std::shared_ptr<Material>> materials;
	std::map<std::string, std::shared_ptr<Mesh>> meshes;
	std::map<std::string, std::shared_ptr<Font>> fonts;
};

