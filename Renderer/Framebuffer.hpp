#pragma once

#include "Renderbuffer.hpp"
#include "CubemapArray.hpp"
#include "Cubemap.hpp"
#include "Texture.hpp"

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();
	void AttachTexture(Texture& texture, GLenum attachmentType);
	void AttachRenderbuffer(Renderbuffer& renderbuffer, GLenum attachmentType);
	void AttachCubemapFace(Cubemap& cubemap, unsigned int face, GLenum attachmentType, unsigned int mipMapLevel);
	void AttachCubemapArrayFace(CubemapArray& cubemapArray, unsigned int index, unsigned int face, GLenum attachmentType, unsigned int mipMapLevel);
	void Bind();
	int GetID();
	static void Unbind();
private:
	unsigned int frameBufferObjectID;
};