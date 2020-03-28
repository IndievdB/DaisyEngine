#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
	Texture(float width, float height, GLenum internalFormat, GLenum format);
	Texture(std::string filePath);
	void Bind(int textureUnit);
	unsigned int GetTextureID();
private:
	unsigned int textureID;
};