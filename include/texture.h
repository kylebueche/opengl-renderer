#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

enum TEXTURE_TYPE
{
    diffuse,
    specular
};

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
};

GLint loadTexture(const char* texturePath, std::string directory);

#endif
