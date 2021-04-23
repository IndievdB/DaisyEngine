// ============================================
// Begin Vertex Shader
// ============================================

#version 430 core

in vec3 aPos : POSITION;
in vec2 aTexCoords : TEXCOORD;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec3 ModelPos;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	ModelPos = aPos;
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 430 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 ModelPos;
uniform sampler2D mainTex;

uniform samplerCube cubemap;
uniform samplerCubeArray cubemapArray;

void main()
{
	FragColor = vec4(1,1,1,1);
	
	float closetDepth = texture(cubemapArray, vec4(ModelPos, 0)).r;
	//vec3 envColor = texture(cubemapArray, ModelPos).rgb;

    // HDR tonemap and gamma correct
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2));

    FragColor = vec4(vec3(closetDepth), 1.0);
}