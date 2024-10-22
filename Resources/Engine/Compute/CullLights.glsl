#version 430 core

const int numLights = 256;
const int numClusters = 512;

struct Cluster
{
    vec4 minBounds;
    vec4 maxBounds;
};

struct PointLight
{
	vec4 position;
	vec4 color;
	float radius;
	float intensity;
	float pl_padding[2];
};

struct SpotLight
{
	vec4 position;
	vec4 direction;
	vec4 color;
	float intensity;
	float cutOff;
	float range;
	float sl_padding[1];
};

layout (std430, binding = 0) buffer ClusterBuffer
{
	Cluster clusters[];
};

layout(std430, binding = 1) buffer PointLightBuffer
{
	PointLight pointLights[numLights];
	int clusterPointLightCount[numClusters];
	int clusterPointLightIndices[numClusters][numLights];
};

layout(std430, binding = 2) buffer SpotLightBuffer
{
	SpotLight spotLights[numLights];
	int clusterSpotLightCount[numClusters];
	int clusterSpotLightIndices[numClusters][numLights];
};

uniform uvec3 clusterSizes;
uniform int numPointLights;
uniform int numSpotLights;
uniform mat4 view;

bool PointLightIntersectsCluster(vec3 pointLightPosition, float pointLightRadius, Cluster cluster)
{
	vec3 closest = max(cluster.minBounds.xyz, min(pointLightPosition, cluster.maxBounds.xyz));
	vec3 dist = closest - pointLightPosition;
	return dot(dist, dist) <= (pointLightRadius * pointLightRadius);
}

bool SpotLightIntersectsCluster(vec3 position, vec3 direction, float range, float angle, Cluster cluster)
{
	vec3 extents = abs((cluster.maxBounds.xyz - cluster.minBounds.xyz) / 2.0);
	vec3 center = cluster.minBounds.xyz + extents;
	float sphereRadius = sqrt(dot(extents, extents));

    vec3 v = center - position;
    float lenSq = dot(v, v);
    float v1Len = dot(v, direction);
    float distanceClosestPoint = cos(angle) * sqrt(lenSq - v1Len * v1Len) - v1Len * tan(angle);
    bool angleCull = distanceClosestPoint > sphereRadius;
    bool frontCull = v1Len > sphereRadius + range;
    bool backCull = v1Len < -sphereRadius;
	return !(angleCull || frontCull || backCull);
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
	uint clusterIndex = gl_GlobalInvocationID.z * clusterSizes.x * clusterSizes.y +
                        gl_GlobalInvocationID.y * clusterSizes.x +
                        gl_GlobalInvocationID.x;

	/*vec3 min = vec3(-0.5f,-0.5f,-0.5f);
	vec3 max = vec3(0.5f, 0.5f, 0.5f);

	SpotLight spotLight = spotLights[0];

	vec3 spotLightPositionView = (view * vec4(spotLight.position.xyz,1)).xyz;
	spotLightPositionView.z = -spotLightPositionView.z;

	vec3 spotLightDirectionView = (view * vec4(spotLight.direction.xyz,0)).xyz;
	spotLightDirectionView.z = -spotLightDirectionView.z;
	spotLightDirectionView = normalize(spotLightDirectionView);

	
	p = spotLight.position;
	d = normalize(spotLight.direction);
	viewP = vec4(spotLightPositionView,1);
	viewD = vec4(spotLightDirectionView,1);*/

	Cluster cluster = clusters[clusterIndex];

	int intersections = 0;

	for (int i=0; i < numPointLights; i++)
	{
		PointLight pointLight = pointLights[i];

		vec3 pointLightPosition = pointLight.position.xyz;
		vec3 pointLightPositionView = (view * vec4(pointLightPosition,1)).xyz;
		pointLightPositionView.z = -pointLightPositionView.z;

		if (PointLightIntersectsCluster(pointLightPositionView, pointLight.radius, cluster))
		{
			clusterPointLightIndices[clusterIndex][intersections] = i;
			++intersections;
		}
	}

	clusterPointLightCount[clusterIndex] = intersections;

	intersections = 0;

	for (int i=0; i < numSpotLights; i++)
	{
		SpotLight spotLight = spotLights[i];

		vec3 spotLightPositionView = (view * vec4(spotLight.position.xyz,1)).xyz;
		spotLightPositionView.z = -spotLightPositionView.z;

		vec3 spotLightDirectionView = (view * vec4(spotLight.direction.xyz,0)).xyz;
		spotLightDirectionView.z = -spotLightDirectionView.z;
		spotLightDirectionView = normalize(spotLightDirectionView);
		
		if (SpotLightIntersectsCluster(spotLightPositionView, spotLightDirectionView, spotLight.range, radians(spotLight.cutOff), cluster))
		{
			clusterSpotLightIndices[clusterIndex][intersections] = i;
			++intersections;
		}
	}

	clusterSpotLightCount[clusterIndex] = intersections;
}
