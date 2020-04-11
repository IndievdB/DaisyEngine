// ============================================
// Begin Vertex Shader
// ============================================

#version 430 core

in vec3 aPos : POSITION;
in vec2 aTexCoords : TEXCOORD;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 1.0);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 430 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D mainTex;

void main()
{
	FragColor = texture(mainTex, TexCoords);
}