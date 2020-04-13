#pragma once

#include <glad/glad.h>

class GLUtil
{
public:
	template <class T>
	static GLuint InitSSBO(int numBuffers, int binding, GLuint bufferID, size_t size, T* data, GLenum usage)
	{
		glGenBuffers(numBuffers, &bufferID);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, bufferID);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return bufferID;
	}

	template <class T>
	static GLuint RebufferData(const GLenum& target, const GLuint& bufferID, const size_t& offset, const size_t& size, const T* data)
	{
		glBindBuffer(target, bufferID);
		glBufferSubData(target, offset, size, data);
		glBindBuffer(target, 0);

		return bufferID;
	}
};

