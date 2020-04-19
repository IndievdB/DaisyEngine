// ============================================
// Begin Vertex Shader
// ============================================

#version 430 core
in vec3 aPos : POSITION;
in vec2 aTexCoord : TEXCOORD;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 5) in ivec4 a_BoneIndices;
layout(location = 6) in vec4 a_BoneWeights;

const int MAX_BONES = 100;
uniform mat4 u_BoneTransforms[100];

void main()
{
	mat4 boneTransform = u_BoneTransforms[a_BoneIndices[0]] * a_BoneWeights[0];
	boneTransform += u_BoneTransforms[a_BoneIndices[1]] * a_BoneWeights[1];
	boneTransform += u_BoneTransforms[a_BoneIndices[2]] * a_BoneWeights[2];
	boneTransform += u_BoneTransforms[a_BoneIndices[3]] * a_BoneWeights[3];

	gl_Position = projection * view * model * boneTransform * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D mainTex;

void main()
{
	FragColor = texture(mainTex, TexCoord);
}