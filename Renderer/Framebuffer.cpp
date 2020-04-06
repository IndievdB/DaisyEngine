#include "Framebuffer.hpp"

#include <glad/glad.h>

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &frameBufferObjectID);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &frameBufferObjectID);
}

void Framebuffer::AttachRenderbuffer(Renderbuffer& renderbuffer, GLenum attachmentType)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, renderbuffer.GetBufferID());
}

void Framebuffer::AttachTexture(Texture& texture, GLenum attachmentType)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, texture.GetTextureID(), 0);
}

void Framebuffer::AttachCubemapFace(Cubemap& cubemap, unsigned int face, GLenum attachmentType, unsigned int mipMapLevel)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubemap.GetCubemapID(), mipMapLevel);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}