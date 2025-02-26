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

//out vec4 FragColor;

void main()
{             
	//FragColor = vec4(1,1,1,1);
    // gl_FragDepth = gl_FragCoord.z;
	
}