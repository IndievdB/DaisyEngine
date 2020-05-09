#include "Renderbuffer.hpp"

Renderbuffer::Renderbuffer(GLenum format, int width, int height)
{
	glGenRenderbuffers(1, &renderBufferObjectID);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObjectID);
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

Renderbuffer::~Renderbuffer()
{
	glDeleteRenderbuffers(1, &renderBufferObjectID);
}

unsigned int Renderbuffer::GetBufferID()
{
	return renderBufferObjectID;
}

void Renderbuffer::Reformat(GLenum format, int width, int height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObjectID);
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}