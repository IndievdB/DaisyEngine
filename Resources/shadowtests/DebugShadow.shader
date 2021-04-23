// ============================================
// Begin Vertex Shader
// ============================================

#version 430
in vec3 aPos : POSITION;
in vec2 aTexCoords : TEXCOORD;
in vec3 aNormal : NORMAL;

out vec3 ViewPos;
out vec2 TexCoords;
out vec3 VSNormal;
out vec3 WSNormal;
out vec3 WorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	WorldPos = vec3(model * vec4(aPos, 1.0));

	vec4 viewPos = view * model * vec4(aPos, 1.0);
	ViewPos = viewPos.xyz;
	TexCoords = aTexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	VSNormal = normalMatrix * (vec4(aNormal, 1.0)).xyz;
	WSNormal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * viewPos;
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 ViewPos;
in vec2 TexCoords;
in vec3 VSNormal;
in vec3 WSNormal;

uniform float nearPlane;
uniform float farPlane;
uniform float screenWidth;
uniform float screenHeight;
uniform float ambientLighting;
uniform mat4 view;
uniform sampler2D mainTex;
uniform sampler2D shadowMap;
uniform samplerCube pointShadowMap;
uniform vec3 camPos;

vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float ShadowCalculationPoint(vec3 fragPos, vec3 lightPos, float lightRadius)
{
	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(ViewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(pointShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= lightRadius;   // undo mapping [0;1]
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

	//shadow = currentDepth - bias > texture(pointShadowMap, fragToLight).r * farPlane ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	float shadow = ShadowCalculationPoint(WorldPos, vec3(0,3,0), 5);
	FragColor = vec4(vec3(shadow), 1);// texture(mainTex, TexCoord);
}