// ============================================
// Begin Vertex Shader
// ============================================

#version 330 core
in vec3 aPos : POSITION;
in vec2 aTexCoord : TEXCOORD;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D mainTex;

void main()
{
	FragColor = texture(mainTex, TexCoord);
}