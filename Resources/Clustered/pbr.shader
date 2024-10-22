// ============================================
// Begin Vertex Shader
// ============================================

#version 430
in vec3 aPos : POSITION;
in vec2 aTexCoords : TEXCOORD;
in vec3 aNormal : NORMAL;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 WorldPos;

void main()
{
	/*vec4 viewPos = view * model * vec4(aPos, 1.0);
	FragPos = viewPos.xyz;
	TexCoords = aTexCoords;

	WorldPos = vec3(model * vec4(aPos, 1.0));
	
	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	Normal = normalMatrix * (vec4(aNormal, 1.0)).xyz;
	
	gl_Position = projection * viewPos;*/

	TexCoords = aTexCoords;
	WorldPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(model) * aNormal;

	gl_Position = projection * view * vec4(WorldPos, 1.0);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 430

const int numLights = 512;
const vec3 tilesOnAxes = vec3(8, 8, 8);
const int numTiles = int(tilesOnAxes.x * tilesOnAxes.y * tilesOnAxes.z);

int GetTileIndex(const int xIndex, const int yIndex, const int zIndex)
{
	return xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);
}

struct LightData
{
	vec4 pos4;
	vec4 lightColour;
	float lightRadius;
	float intensity;

	float fpadding[2];
};

struct Tile
{
	float x;
	float y;
	float z;
	float width;
	float height;
	float length;

	float _padding[6];
};

layout (std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
};

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[][numLights];
};

layout(binding = 0) uniform atomic_uint count;

uniform float nearPlane;
uniform float farPlane;
uniform float ambientLighting;
uniform mat4 view;
uniform sampler2D mainTex;

//PBR
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform vec3 camPos;
const float PI = 3.14159265359;
//

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 WorldPos;

out vec4 FragColor;

// ----------------------------------------------------------------------------
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(WorldPos);
	vec3 Q2 = dFdy(WorldPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);

	vec3 N = normalize(Normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

vec4 AddPBRLighting(int tileIndex)
{
	// material properties
	vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	float ao = texture(aoMap, TexCoords).r;

	// input lighting data
	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - WorldPos);
	vec3 R = reflect(-V, N);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);

	for (int i = 0; i < lightIndexes[tileIndex]; i++)
	{
		int lightIndex = tileLights[tileIndex][i];

		// calculate per-light radiance
		vec3 L = normalize(lightData[lightIndex].pos4.xyz - WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightData[lightIndex].pos4.xyz - WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightData[lightIndex].lightColour.rgb * attenuation * pow(lightData[lightIndex].intensity, 2.2f);

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
		vec3 specular = nominator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}

	// ambient lighting (we now use IBL as the ambient term)
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;

	// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	//vec3 color = Lo;
	vec3 color = ambient + Lo;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));

	return vec4(color, 1.0);
}


void main(void)
{
	float zCoord = abs(FragPos.z - nearPlane) / (farPlane - nearPlane);
	int xIndex = int(TexCoords.x * (tilesOnAxes.x - 1));
	int yIndex = int(TexCoords.y * (tilesOnAxes.y - 1));
	int zIndex = int(zCoord * (tilesOnAxes.z - 1));
	int tile = GetTileIndex(xIndex, yIndex, zIndex);

	FragColor = AddPBRLighting(tile);
}