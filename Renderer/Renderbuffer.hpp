#pragma once

#include <glad/glad.h>

class Renderbuffer
{
public:
	Renderbuffer(GLenum format, int width, int height);
	~Renderbuffer();
	unsigned int GetBufferID();
	void Reformat(GLenum format, int width, int height);
private:
	unsigned int renderBufferObjectID;
};