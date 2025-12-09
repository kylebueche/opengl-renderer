#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

#include "glm/vec3.hpp"

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;

    Texture() = default;
    Texture(glm::vec3 color);
    Texture(GLuint texture, std::string type, std::string path);
};

GLint loadTexture(const char* texturePath, std::string directory);
GLint colorTexture(glm::vec3 color);

#endif
