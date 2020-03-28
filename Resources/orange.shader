// ============================================
// Begin Vertex Shader
// ============================================

#version 330 core
in vec3 aPos : POSITION;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

// ============================================
// Begin Fragment Shader
// ============================================

#version 330 core
out vec4 FragColor;
void main()
{
   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}