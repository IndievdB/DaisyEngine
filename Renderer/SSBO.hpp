#pragma once

#include <glad/glad.h>

class SSBO
{
public:
	template <class T>
	void Initialize(int binding, size_t size, T* data)
	{
		glGenBuffers(1, &ID);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ID);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	};
private:
	GLuint ID;
};