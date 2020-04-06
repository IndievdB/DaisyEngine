#ifndef Cubemap_hpp
#define Cubemap_hpp

#include <glad/glad.h>
#include <string>

class Cubemap
{
public:
	Cubemap(GLenum minificationFilter, int width, int height);
	unsigned int GetCubemapID();
	void GenerateMipMaps();
	void Bind(int textureUnit);
private:
	unsigned int cubemapID;
};


#endif