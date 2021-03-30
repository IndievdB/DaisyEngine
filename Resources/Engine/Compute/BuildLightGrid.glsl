#version 430 core

struct Cluster
{
    vec4 minBounds;
    vec4 maxBounds;
};

layout (std430, binding = 0) buffer ClusterBuffer
{
	Cluster clusters[];
};

uniform vec2 clusterScreenSpaceSize;
uniform float screenWidth;
uniform float screenHeight;
uniform mat4 inverseProjection;
uniform float nearPlane;
uniform float farPlane;
uniform uvec3 clusterSizes;

vec4 Screen2Eye(vec4 coord)
{
    vec3 ndc = vec3(
        2.0 * (coord.x - 0.0) / screenWidth - 1.0,
        2.0 * (coord.y - 0.0) / screenHeight - 1.0,
        -2.0 * coord.z - 1.0
    );

    vec4 eye = inverseProjection * vec4(ndc, 1.0);
    eye = eye / eye.w;

    return eye;
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
	uint clusterIndex = gl_GlobalInvocationID.z * clusterSizes.x * clusterSizes.y +
                        gl_GlobalInvocationID.y * clusterSizes.x +
                        gl_GlobalInvocationID.x;

	vec4 minScreen = vec4(gl_GlobalInvocationID.xy * clusterScreenSpaceSize, 1.0, 1.0);
    vec4 maxScreen = vec4((gl_GlobalInvocationID.xy + vec2(1, 1)) * clusterScreenSpaceSize, 1.0, 1.0);

    vec3 minEye = Screen2Eye(minScreen).xyz;
    vec3 maxEye = Screen2Eye(maxScreen).xyz;

    float clusterNear = nearPlane * pow(farPlane / nearPlane,  gl_GlobalInvocationID.z      / float(clusterSizes.z));
    float clusterFar  = nearPlane * pow(farPlane / nearPlane, (gl_GlobalInvocationID.z + 1) / float(clusterSizes.z));

    vec3 minNear = minEye * clusterNear / minEye.z;
    vec3 minFar  = minEye * clusterFar  / minEye.z;
    vec3 maxNear = maxEye * clusterNear / maxEye.z;
    vec3 maxFar  = maxEye * clusterFar  / maxEye.z;

    vec3 minBounds = min(min(minNear, minFar), min(maxNear, maxFar));
    vec3 maxBounds = max(max(minNear, minFar), max(maxNear, maxFar));

    clusters[clusterIndex].minBounds = vec4(minBounds, 1.0);
    clusters[clusterIndex].maxBounds = vec4(maxBounds, 1.0);
}
