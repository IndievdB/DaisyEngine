// ============================================
// Begin Vertex Shader
// ============================================

#version 330 core
in vec3 aPos : POSITION;
in vec2 aTexCoord : TEXCOORD;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

void main()
{
	FragColor = vec4(1,1,1,1);
}