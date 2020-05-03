#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
	Texture(float width, float height, GLenum internalFormat, GLenum format, GLenum minMagFilter = GL_LINEAR, GLenum textureWrap = GL_CLAMP_TO_EDGE);
	Texture(std::string filePath);
	void Bind(int textureUnit);
	unsigned int GetTextureID();
	void AddBorder(float r, float g, float b, float a);
private:
	unsigned int textureID;
};