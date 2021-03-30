#pragma once

#include <glad/glad.h>
#include <cstddef>

class AtomicCounterBuffer
{
public:
	void Initialize(int binding, int numberOfCounters)
	{
		this->numberOfCounters = numberOfCounters;
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ID);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * numberOfCounters, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, binding, ID);
	}
	void SetAll(int value)
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ID);
		GLuint data[3] = { value, value, value };
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * numberOfCounters, data);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}
	void Set(int index, int value)
	{
		if (index >= numberOfCounters)
			return;

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ID);
		GLuint data = value;
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, index*sizeof(GLuint), sizeof(GLuint), &data);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}
private:
	GLuint ID;
	int numberOfCounters;
};