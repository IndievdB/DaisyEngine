#include "ComputeShader.hpp"
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>

ComputeShader::ComputeShader(std::string filePath)
{
	shader = GenerateShader(filePath);
	program = glCreateProgram();
	glAttachShader(program, shader);
	Link();
}

ComputeShader::~ComputeShader()
{
	glDetachShader(program, shader);
	glDeleteShader(shader);
	glDeleteProgram(program);
}

bool ComputeShader::Link()
{
	glLinkProgram(program);

	GLint code;
	glGetProgramiv(program, GL_LINK_STATUS, &code);
	return code == GL_TRUE ? true : false;
}


void ComputeShader::Use()
{
	glUseProgram(program);
}

const char* GetGLErrorStr(GLenum err)
{
	switch (err)
	{
	case GL_NO_ERROR:          return "No error";
	case GL_INVALID_ENUM:      return "Invalid enum";
	case GL_INVALID_VALUE:     return "Invalid value";
	case GL_INVALID_OPERATION: return "Invalid operation";
	case GL_STACK_OVERFLOW:    return "Stack overflow";
	case GL_STACK_UNDERFLOW:   return "Stack underflow";
	case GL_OUT_OF_MEMORY:     return "Out of memory";
	default:                   return "Unknown error";
	}
}

void ComputeShader::Dispatch(int x, int y, int z)
{
	//std::cout << GetGLErrorStr(glGetError()) << std::endl;
	Use();
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//std::cout << GetGLErrorStr(glGetError()) << std::endl;
}

int ComputeShader::GenerateShader(std::string filePath)
{
	std::string content;
	LoadShaderFile(filePath, content);

	shader = glCreateShader(GL_COMPUTE_SHADER);
	const char* chars = content.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);
	CheckCompileErrors(shader);

	return shader;
}

bool ComputeShader::LoadShaderFile(std::string from, std::string& into)
{
	std::ifstream file;
	std::string temp;

	file.open(from.c_str());

	if (!file.is_open())
	{
		return false;
	}

	while (!file.eof())
	{
		getline(file, temp);

		if (temp.find("#include") != std::string::npos)
		{
			into += IncludeShader(temp) + "\n";
		}
		else
		{
			into += temp + "\n";
		}
	}

	file.close();
	return true;
}

std::string ComputeShader::IncludeShader(std::string includeLine)
{
	std::istringstream iss(includeLine);

	std::vector<std::string> tokens
	{
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{}
	};

	std::string glslToAppend;
	LoadShaderFile(tokens.at(1), glslToAppend);

	return glslToAppend;
}

void ComputeShader::CheckCompileErrors(unsigned int inputShader)
{
	int success;
	char infoLog[1024];

	glGetShaderiv(inputShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(inputShader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: COMPUTE\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
}

void ComputeShader::SetBool(std::string name, bool value) const
{
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void ComputeShader::SetInt(std::string name, int value) const
{
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void ComputeShader::SetFloat(std::string name, float value) const
{
	glUseProgram(program);
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void ComputeShader::SetUVector3(std::string name, uint32_t x, uint32_t y, uint32_t z) const
{
	glUseProgram(program);
	glUniform3ui(glGetUniformLocation(program, name.c_str()), x, y, z);
}

void ComputeShader::SetVector2(std::string name, float x, float y) const
{
	glUseProgram(program);
	glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
}

void ComputeShader::SetVector3(std::string name, float x, float y, float z) const
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
}

void ComputeShader::SetVector4(std::string name, float x, float y, float z, float w) const
{
	glUseProgram(program);
	glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
}

void ComputeShader::SetMatrix4x4(std::string name, const Matrix4x4& matrix) const
{
	glUseProgram(program);
	unsigned int transformLoc = glGetUniformLocation(program, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, matrix.mV);
}