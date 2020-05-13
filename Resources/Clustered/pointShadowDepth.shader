// ============================================
// Begin Vertex Shader
// ============================================

#version 330
in vec3 aPos : POSITION;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec4 FragPos;

void main()
{
	FragPos = model * vec4(aPos, 1.0);
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float lightRadius;

out vec4 FragColor;

void main()
{             
    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance = lightDistance / lightRadius;
	FragColor = vec4(vec3(lightDistance),1);
}