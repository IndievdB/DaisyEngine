// ============================================
// Begin Vertex Shader
// ============================================

#version 330 core
in vec3 aPos : POSITION;
in vec2 aTexCoord : TEXCOORD;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(aPos.xy, 0.0, 1.0);
    TexCoord = aTexCoord.xy;
}  

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core
in vec2 TexCoord;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoord).r);
    color = vec4(textColor, 1.0) * sampled;
	//color = vec4(1.0);
}  