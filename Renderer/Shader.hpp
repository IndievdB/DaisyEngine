#ifndef Shader_hpp
#define Shader_hpp

#include <string>
#include <unordered_map>

#include "../Core/Math/Matrix4x4.hpp"

class Shader
{
public:
	Shader(const std::string& shaderPath);
	void Use();
	void SetBool(std::string name, bool value);
	void SetInt(std::string name, int value);
	void SetFloat(std::string name, float value);
	void SetVector2(std::string name, float x, float y);
	void SetVector3(std::string name, float x, float y, float z);
	void SetVector4(std::string name, float x, float y, float z, float w);
	void SetMatrix4x4(std::string name, Matrix4x4 matrix);
	int GetTextureUnit(std::string name);
	bool HasUniform(std::string name);
private:
	void CheckCompileErrors(unsigned int shader, std::string type);
	void Compile(const std::string& vertexSource, const std::string& framentSource);
	int shaderProgramID;
	std::unordered_map<std::string, int> textureUnitMap;
	std::unordered_map<std::string, int> uniformMap;
};

#endif
