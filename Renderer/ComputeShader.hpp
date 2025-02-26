#pragma once

#include <iostream>
#include "../Core/Math/Matrix4x4.hpp"

class ComputeShader
{
public:
	ComputeShader(std::string filePath);
	~ComputeShader();
	bool Link();
	void Use();
	void Dispatch(int x, int y, int z);
	void SetBool(std::string name, bool value) const;
	void SetInt(std::string name, int value) const;
	void SetFloat(std::string name, float value) const;
	void SetUVector3(std::string name, uint32_t x, uint32_t y, uint32_t z) const;
	void SetVector2(std::string name, float x, float y) const;
	void SetVector3(std::string name, float x, float y, float z) const;
	void SetVector4(std::string name, float x, float y, float z, float w) const;
	void SetMatrix4x4(std::string name, const Matrix4x4& matrix) const;
	uint32_t GetProgramID() { return program; };
 private:
	int GenerateShader(std::string filePath);
	bool LoadShaderFile(std::string from, std::string& into);
	std::string IncludeShader(std::string includeLine);
	void CheckCompileErrors(unsigned int shader);
	uint32_t program;
	uint32_t shader;
};