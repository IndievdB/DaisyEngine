// ============================================
// Begin Vertex Shader
// ============================================

#version 330
in vec3 aPos : POSITION;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core

void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}