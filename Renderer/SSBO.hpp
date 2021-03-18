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

		initialized = true;
	};

	template <class T>
	void Set(int binding, size_t size, T* data)
	{
		if (initialized == false)
		{
			Initialize(binding, size, data);
			return;
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	};

	bool initialized = false;
private:
	GLuint ID;
};
