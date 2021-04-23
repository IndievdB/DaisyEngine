#include "CubemapArray.hpp"
#include <iostream>

CubemapArray::CubemapArray(int resolution)
{
	glGenTextures(1, &cubemapArrayID);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubemapArrayID);
    int maxPointLights = 256;
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, resolution, resolution, maxPointLights * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //linear
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //linear

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
}

void CubemapArray::Bind(int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubemapArrayID);
}

unsigned int CubemapArray::GetID()
{
    return cubemapArrayID;
}
