#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <glad/glad.h>

Shader::Shader(const std::string& shaderPath)
{
	std::ifstream file(shaderPath, std::ios::in | std::ios::binary);

	if (!file)
	{
		std::cout << "failed to load shader" << std::endl;
		return;
	}

	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::string delimiter = "// Begin Fragment Shader";
	int delimiterIndex = (int)contents.find(delimiter);

	std::string vertexSource = contents.substr(0, delimiterIndex);
	std::string fragementSource = contents.substr(delimiterIndex, contents.length());

	size_t index = 0;

	index = vertexSource.find(" : POSITION");
	if (index != std::string::npos)
	{
		vertexSource.replace(index, 11, "");
		while (index != 0 && vertexSource[index - 1] != '\n') index--;
		vertexSource.insert(index, "layout(location = 0) ");
	}

	index = vertexSource.find(" : NORMAL");
	if (index != std::string::npos)
	{
		vertexSource.replace(index, 9, "");
		while (index != 0 && vertexSource[index - 1] != '\n') index--;
		vertexSource.insert(index, "layout(location = 1) ");
	}

	index = vertexSource.find(" : TEXCOORD");
	if (index != std::string::npos)
	{
		vertexSource.replace(index, 11, "");
		while (index != 0 && vertexSource[index - 1] != '\n') index--;
		vertexSource.insert(index, "layout(location = 4) ");
	}
	
	Compile(vertexSource, fragementSource);
}

void Shader::Use()
{
	glUseProgram(shaderProgramID);
}

void Shader::Compile(const std::string& vertexSource, const std::string& fragementSource)
{
	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexSourceCstr = (const GLchar*)vertexSource.c_str();
	glShaderSource(vertex, 1, &vertexSourceCstr, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragmentSourceCstr = (const GLchar*)fragementSource.c_str();
	glShaderSource(fragment, 1, &fragmentSourceCstr, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertex);
	glAttachShader(shaderProgramID, fragment);
	glLinkProgram(shaderProgramID);
	CheckCompileErrors(shaderProgramID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(shaderProgramID);

	std::string uniformSamplerText = "uniform sampler";
	size_t index = fragementSource.find(uniformSamplerText);
	int textureUnitIndex = 0;

	while (index != std::string::npos)
	{
		index = fragementSource.find(' ', index + 10);

		size_t semiColonIndex = fragementSource.find(";", index);
		size_t start = index + 1;
		size_t length = semiColonIndex - start;

		std::string texName = fragementSource.substr(start, length);

		glUniform1i(glGetUniformLocation(shaderProgramID, texName.c_str()), textureUnitIndex);
		textureUnitMap.emplace(texName, textureUnitIndex);
		textureUnitIndex++;

		index = fragementSource.find(uniformSamplerText, index);
	}

	glUseProgram(0);
}

void Shader::CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void Shader::SetBool(std::string name, bool value)
{
	glUseProgram(shaderProgramID);

	auto iterator = uniformMap.find(name);
	int uniformLocation;

	if (iterator == uniformMap.end())
	{
		uniformLocation = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap.emplace(name, uniformLocation);
	}
	else
	{
		uniformLocation = iterator->second;
	}

	glUniform1i(uniformLocation, (int)value);
}

void Shader::SetInt(std::string name, int value)
{
	glUseProgram(shaderProgramID);

	auto iterator = uniformMap.find(name);
	int uniformLocation;

	if (iterator == uniformMap.end())
	{
		uniformLocation = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap.emplace(name, uniformLocation);
	}
	else
	{
		uniformLocation = iterator->second;
	}

	glUniform1i(uniformLocation, value);
}

void Shader::SetFloat(std::string name, float value)
{
	glUseProgram(shaderProgramID);

	auto iterator = uniformMap.find(name);
	int uniformLocation;

	if (iterator == uniformMap.end())
	{
		uniformLocation = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap.emplace(name, uniformLocation);
	}
	else
	{
		uniformLocation = iterator->second;
	}

	glUniform1f(uniformLocation, value);
}

void Shader::SetVector2(std::string name, float x, float y)
{
	glUseProgram(shaderProgramID);

	auto iterator = uniformMap.find(name);
	int uniformLocation;

	if (iterator == uniformMap.end())
	{
		uniformLocation = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap.emplace(name, uniformLocation);
	}
	else
	{
		uniformLocation = iterator->second;
	}

	glUniform2f(uniformLocation, x, y);
}

void Shader::SetVector3(std::string name, float x, float y, float z)
{
	glUseProgram(shaderProgramID);

	auto iterator = uniformMap.find(name);
	int uniformLocation;

	if (iterator == uniformMap.end())
	{
		uniformLocation = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap.emplace(name, uniformLocation);
	}
	else
	{
		uniformLocation = iterator->second;
	}

	glUniform3f(uniformLocation, x, y, z);
}

void Shader::SetVector4(std::string name, float x, float y, float z, float w)
{
	glUseProgram(shaderProgramID);

	auto iterator = uniformMap.find(name);
	int uniformLocation;

	if (iterator == uniformMap.end())
	{
		uniformLocation = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap.emplace(name, uniformLocation);
	}
	else
	{
		uniformLocation = iterator->second;
	}

	glUniform4f(uniformLocation, x, y, z, w);
}

void Shader::SetMatrix4x4(std::string name, Matrix4x4 matrix)
{
	glUseProgram(shaderProgramID);

	auto iterator = uniformMap.find(name);
	int uniformLocation;

	if (iterator == uniformMap.end())
	{
		uniformLocation = glGetUniformLocation(shaderProgramID, name.c_str());
		uniformMap.emplace(name, uniformLocation);
	}
	else
	{
		uniformLocation = iterator->second;
	}

	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, matrix.mV);
}

bool Shader::HasUniform(std::string name)
{
	return glGetUniformLocation(shaderProgramID, name.c_str()) != -1;
}

int Shader::GetTextureUnit(std::string name)
{
	auto result = textureUnitMap.find(name);

	return result == textureUnitMap.end() ? -1 : result->second;
}