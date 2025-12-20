#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

#include "glm/vec3.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class Texture;

typedef int TextureID;

struct TextureManager
{
    // Allegedly we want the fast lookup of a vector, (runtime)
    // but also the convenience of a hashtable (loadtime)
    std::vector<Texture> textures;
    std::unordered_map<std::string, TextureID> textureMap;

    void loadTexture(std::string filename);
    TextureID getTextureID(std::string filename);
    float gigabytesLoadedToRAM();
    float gigabytesLoadedToVRAM();
};

class Texture
{
public:
    int width;
    int height;
    int nrChannels;

    // Is resident of GPU memory?
    bool isResident;
    // Fun fact: 1GB = 256 4-channel 1024x1024 textures

    unsigned char *texData;

    Texture();
    Texture(std::string filename);
    ~Texture();

    void loadFromFile(std::string filename);
    void loadToGPU();
    void unloadFromGPU();

    GLuint id;
};

#endif
