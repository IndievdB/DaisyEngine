#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "../Core/Math/Vector2.hpp"
#include "../Core/Math/Vector3.hpp"
#include "../Core/Math/Matrix4x4.hpp"
#include "../Core/Math/Quaternion.hpp"

#include "Shader.hpp"

struct Vertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 Binormal;
	Vector2 Texcoord;
};

struct AnimatedVertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 Binormal;
	Vector2 Texcoord;

	uint32_t IDs[4] = { 0, 0,0, 0 };
	float Weights[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

	void AddBoneData(uint32_t BoneID, float Weight)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (Weights[i] == 0.0)
			{
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}
	}
};

struct Triangle
{
	uint32_t v1, v2, v3;
};

struct BoneInfo
{
	Matrix4x4 BoneOffset;
	Matrix4x4 FinalTransformation;
};

struct VertexBoneData
{
	uint32_t IDs[4];
	float Weights[4];

	VertexBoneData()
	{
		memset(IDs, 0, sizeof(IDs));
		memset(Weights, 0, sizeof(Weights));
	};

	void AddBoneData(uint32_t BoneID, float Weight)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (Weights[i] == 0.0)
			{
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}
	}
};

class Submesh
{
public:
	uint32_t BaseVertex;
	uint32_t BaseIndex;
	uint32_t MaterialIndex;
	uint32_t IndexCount;

	Matrix4x4 Transform;
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



public:
	void Render(std::shared_ptr<Shader> shader, float deltaTime);
private:
	void BoneTransform(float time);
	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4x4& ParentTransform);
	void TraverseNodes(aiNode* node, int level = 0);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
	uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	Vector3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
	Quaternion InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
	Vector3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);
private:
	Matrix4x4 m_InverseTransform;
	uint32_t m_BoneCount = 0;
	std::vector<BoneInfo> m_BoneInfo;
	std::vector<AnimatedVertex> m_AnimatedVertices;
	std::unordered_map<std::string, uint32_t> m_BoneMapping;
	std::vector<Matrix4x4> m_BoneTransforms;
	bool m_IsAnimated = false;
	float m_AnimationTime = 0.0f;
	float m_WorldTime = 0.0f;
	float m_TimeMultiplier = 1.0f;
	bool m_AnimationPlaying = true;
};