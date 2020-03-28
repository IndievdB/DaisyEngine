#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Binormal;
	glm::vec2 Texcoord;
};

struct Triangle
{
	uint32_t v1, v2, v3;
};

class Submesh
{
public:
	uint32_t BaseVertex;
	uint32_t BaseIndex;
	uint32_t MaterialIndex;
	uint32_t IndexCount;

	glm::mat4 Transform;
};

class Mesh
{
public:
	Mesh(const std::string& filename);
	void Render();
private:
	std::unique_ptr<Assimp::Importer> importer;
	std::vector<Submesh> submeshes;
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	const aiScene* m_Scene;
	std::string m_FilePath;
	uint32_t vertexArrayObjectID = -1;
};

