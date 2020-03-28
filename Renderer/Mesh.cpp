#include "Mesh.hpp"
#include <glad/glad.h>

static const uint32_t s_MeshImportFlags =
aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
aiProcess_Triangulate |             // Make sure we're triangles
aiProcess_SortByPType |             // Split meshes by primitive type
aiProcess_GenNormals |              // Make sure we have legit normals
aiProcess_GenUVCoords |             // Convert UVs if required 
aiProcess_OptimizeMeshes |          // Batch draws where possible
aiProcess_ValidateDataStructure;    // Validation

Mesh::Mesh(const std::string& filename) : m_FilePath(filename)
{
	importer = std::make_unique<Assimp::Importer>();
	const aiScene* scene = importer->ReadFile(filename, s_MeshImportFlags);

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	submeshes.reserve(scene->mNumMeshes);

	for (size_t m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];

		Submesh submesh;
		submesh.BaseVertex = vertexCount;
		submesh.BaseIndex = indexCount;
		submesh.MaterialIndex = mesh->mMaterialIndex;
		submesh.IndexCount = mesh->mNumFaces * 3;
		submeshes.push_back(submesh);

		vertexCount += mesh->mNumVertices;
		indexCount += submesh.IndexCount;

		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
				vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

			vertices.push_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			triangles.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
		}

		if (vertexArrayObjectID >= 0)
			glDeleteProgram(vertexArrayObjectID);

		unsigned int VBO, EBO;

		glGenVertexArrays(1, &vertexArrayObjectID);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(vertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(Triangle), &triangles[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Binormal));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoord));
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_Scene = scene;
}

void Mesh::Render()
{
	glBindVertexArray(vertexArrayObjectID);

	for (Submesh& submesh : submeshes)
	{
		glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
	}

	glBindVertexArray(0);
}