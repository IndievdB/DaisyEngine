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

static Matrix4x4 aiMatrix4x4ToDSY(const aiMatrix4x4& from)
{
	Matrix4x4 result;

	result[0] = from.a1;
	result[1] = from.b1;
	result[2] = from.c1;
	result[3] = from.d1;

	result[4] = from.a2;
	result[5] = from.b2;
	result[6] = from.c2;
	result[7] = from.d2;

	result[8] = from.a3;
	result[9] = from.b3;
	result[10] = from.c3;
	result[11] = from.d3;

	result[12] = from.a4;
	result[13] = from.b4;
	result[14] = from.c4;
	result[15] = from.d4;

	return result;
}

Mesh::Mesh(const std::string& filename) : m_FilePath(filename)
{
	importer = std::make_unique<Assimp::Importer>();
	const aiScene* scene = importer->ReadFile(filename, s_MeshImportFlags);

	if (scene == nullptr)
		std::cout << "File: " << filename << " not found" << std::endl;

	m_IsAnimated = scene->mAnimations != nullptr;
	m_InverseTransform = Matrix4x4::AffineInverse(aiMatrix4x4ToDSY(scene->mRootNode->mTransformation));

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

		if (m_IsAnimated)
		{
			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				AnimatedVertex  vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}

				if (mesh->HasTextureCoords(0))
					vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

				m_AnimatedVertices.push_back(vertex);
			}
		}
		else
		{
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
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			triangles.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
		}

	}

	TraverseNodes(scene->mRootNode);

	if (m_IsAnimated)
	{
		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];
			Submesh& submesh = submeshes[m];

			for (size_t i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* bone = mesh->mBones[i];
				std::string boneName(bone->mName.data);
				int boneIndex = 0;

				if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
				{
					// Allocate an index for a new bone
					boneIndex = m_BoneCount;
					m_BoneCount++;
					BoneInfo bi;
					m_BoneInfo.push_back(bi);
					m_BoneInfo[boneIndex].BoneOffset = aiMatrix4x4ToDSY(bone->mOffsetMatrix);
					m_BoneMapping[boneName] = boneIndex;
				}
				else
				{
					boneIndex = m_BoneMapping[boneName];
				}

				for (size_t j = 0; j < bone->mNumWeights; j++)
				{
					int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
					float Weight = bone->mWeights[j].mWeight;
					m_AnimatedVertices[VertexID].AddBoneData(boneIndex, Weight);
				}
			}
		}
	}
	
	if (vertexArrayObjectID >= 0)
		glDeleteProgram(vertexArrayObjectID);

	unsigned int VBO, EBO;

	glGenVertexArrays(1, &vertexArrayObjectID);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(vertexArrayObjectID);

	if (m_IsAnimated)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_AnimatedVertices.size() * sizeof(AnimatedVertex), &m_AnimatedVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(Triangle), &triangles[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Tangent));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Binormal));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Texcoord));

		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, IDs));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Weights));
	}
	else
	{
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

void Mesh::Render(std::shared_ptr<Shader> shader, float deltaTime)
{
	if (m_IsAnimated)
	{
		if (m_AnimationPlaying)
		{
			m_WorldTime += deltaTime;

			float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * m_TimeMultiplier;
			m_AnimationTime += deltaTime * ticksPerSecond;
			m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
		}

		BoneTransform(m_AnimationTime);
	}

	glBindVertexArray(vertexArrayObjectID);

	for (Submesh& submesh : submeshes)
	{
		if (m_IsAnimated)
		{
			for (size_t i = 0; i < m_BoneTransforms.size(); i++)
			{
				std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
				shader->SetMatrix4x4(uniformName, m_BoneTransforms[i]);
			}
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
	}

	glBindVertexArray(0);
}

void Mesh::TraverseNodes(aiNode* node, int level)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		uint32_t mesh = node->mMeshes[i];
		submeshes[mesh].Transform = aiMatrix4x4ToDSY(node->mTransformation);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		aiNode* child = node->mChildren[i];
		TraverseNodes(child, level + 1);
	}
}

uint32_t Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	return 0;
}


uint32_t Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}

	return 0;
}


uint32_t Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
			return i;
	}

	return 0;
}

Vector3 Mesh::InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumPositionKeys == 1)
	{
		// No interpolation necessary for single value
		auto v = nodeAnim->mPositionKeys[0].mValue;
		return { v.x, v.y, v.z };
	}

	uint32_t PositionIndex = FindPosition(animationTime, nodeAnim);
	uint32_t NextPositionIndex = (PositionIndex + 1);

	float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	if (Factor < 0.0f)
		Factor = 0.0f;

	const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	auto aiVec = Start + Factor * Delta;
	return { aiVec.x, aiVec.y, aiVec.z };
}


Quaternion Mesh::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumRotationKeys == 1)
	{
		// No interpolation necessary for single value
		auto v = nodeAnim->mRotationKeys[0].mValue;
		return Quaternion(v.w, v.x, v.y, v.z);
	}

	uint32_t RotationIndex = FindRotation(animationTime, nodeAnim);
	uint32_t NextRotationIndex = (RotationIndex + 1);

	float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if (Factor < 0.0f)
		Factor = 0.0f;

	const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
	auto q = aiQuaternion();
	aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
	q = q.Normalize();
	return Quaternion(q.w, q.x, q.y, q.z);
}


Vector3 Mesh::InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1)
	{
		// No interpolation necessary for single value
		auto v = nodeAnim->mScalingKeys[0].mValue;
		return { v.x, v.y, v.z };
	}

	uint32_t index = FindScaling(animationTime, nodeAnim);
	uint32_t nextIndex = (index + 1);

	float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
	float factor = (animationTime - (float)nodeAnim->mScalingKeys[index].mTime) / deltaTime;
	if (factor < 0.0f)
		factor = 0.0f;

	const auto& start = nodeAnim->mScalingKeys[index].mValue;
	const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
	auto delta = end - start;
	auto aiVec = start + factor * delta;
	return { aiVec.x, aiVec.y, aiVec.z };
}

void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4x4& ParentTransform)
{
	std::string name(pNode->mName.data);
	const aiAnimation* animation = m_Scene->mAnimations[0];
	Matrix4x4 nodeTransform(aiMatrix4x4ToDSY(pNode->mTransformation));
	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, name);
	
	if (nodeAnim)
	{
		Vector3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
		Matrix4x4 translationMatrix = Matrix4x4::Translation(translation);
	
		Quaternion rotation = InterpolateRotation(AnimationTime, nodeAnim);
		Matrix4x4 rotationMatrix = Matrix4x4(rotation);

		Vector3 scale = InterpolateScale(AnimationTime, nodeAnim);
		Matrix4x4 scaleMatrix = Matrix4x4::Scaling(scale);

		nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
	}

	Matrix4x4 transform = ParentTransform * nodeTransform;

	if (m_BoneMapping.find(name) != m_BoneMapping.end())
	{
		uint32_t BoneIndex = m_BoneMapping[name];
		m_BoneInfo[BoneIndex].FinalTransformation = m_InverseTransform * transform * m_BoneInfo[BoneIndex].BoneOffset;
	}

	for (uint32_t i = 0; i < pNode->mNumChildren; i++)
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
}

const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
{
	for (uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];
		if (std::string(nodeAnim->mNodeName.data) == nodeName)
			return nodeAnim;
	}
	return nullptr;
}

void Mesh::BoneTransform(float time)
{
	ReadNodeHierarchy(time, m_Scene->mRootNode, Matrix4x4());
	m_BoneTransforms.resize(m_BoneCount);
	for (size_t i = 0; i < m_BoneCount; i++)
		m_BoneTransforms[i] = m_BoneInfo[i].FinalTransformation;
}