#ifndef CubemapArray_hpp
#define CubemapArray_hpp

#include <glad/glad.h>
#include <string>

class CubemapArray
{
public:
	CubemapArray(int resolution);
	void Bind(int textureUnit);
	unsigned int GetID();
private:
	unsigned int cubemapArrayID;
};

#endif