// ============================================
// Begin Vertex Shader
// ============================================

#version 330 core
in vec3 aPos : POSITION;
in vec2 aTexCoord : TEXCOORD;
in vec3 aNormal : NORMAL;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	gl_Position = projection * view * vec4(FragPos, 1.0);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

struct DirectionalLight
{
	vec3 direction;
	vec3 color;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 color;

	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

struct PointLight
{
	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 4

uniform sampler2D mainTex;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 AddDirectionalLight(DirectionalLight light, vec3 normal);
vec3 AddSpotLight(SpotLight light, vec3 normal, vec3 fragPos);
vec3 AddPointLight(PointLight light, vec3 normal, vec3 fragPos);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 objectColor = texture(mainTex, TexCoord).xyz;

	vec3 result = 0.1 * vec3(texture(mainTex, TexCoord));

	result += AddDirectionalLight(dirLight, norm);

	result += AddSpotLight(spotLight, norm, FragPos);

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		result += AddPointLight(pointLights[i], norm, FragPos);

	FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 AddDirectionalLight(DirectionalLight light, vec3 normal)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.color * diff * vec3(texture(mainTex, TexCoord));
	return diffuse;
}

// calculates the color when using a spot light.
vec3 AddSpotLight(SpotLight light, vec3 normal, vec3 fragPos)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	vec3 diffuse = light.color * diff * vec3(texture(mainTex, TexCoord));
	diffuse *= attenuation * intensity;
	return diffuse;
}

vec3 AddPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 diffuse = light.color * diff * vec3(texture(mainTex, TexCoord));
	diffuse *= attenuation;
	return diffuse;
}