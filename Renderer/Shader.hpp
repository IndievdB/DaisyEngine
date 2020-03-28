#ifndef Shader_hpp
#define Shader_hpp

#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader(const std::string& shaderPath);
	void Use();
	void SetBool(std::string name, bool value) const;
	void SetInt(std::string name, int value) const;
	void SetFloat(std::string name, float value) const;
	void SetVector3(std::string name, float x, float y, float z) const;
	void SetVector4(std::string name, float x, float y, float z, float w) const;
	void SetMatrix4x4(std::string name, glm::mat4 matrix) const;
	int GetTextureUnit(std::string name);
	bool HasUniform(std::string name);
private:
	void CheckCompileErrors(unsigned int shader, std::string type);
	void Compile(const std::string& vertexSource, const std::string& framentSource);
	int shaderProgramID;
	std::map<std::string, int> textureUnitMap;
};

#endif