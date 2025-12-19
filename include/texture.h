#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

#include "glm/vec3.hpp"
#include <vector>
#include <string>

class Texture
{
public:
    int width;
    int height;
    int nrChannels;

    Texture();
    Texture(std::string filename);
    ~Texture();

    void loadFromFile(std::string filename);

    GLuint id;
};

#endif
