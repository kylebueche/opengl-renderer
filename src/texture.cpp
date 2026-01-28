#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

void TextureManager::loadTexture(std::string filename)
{
    if (!textureMap.contains(filename))
    {
        textures.emplace_back(filename);
        textureMap[filename] = textures.size() - 1;
    }
}

TextureID TextureManager::getTextureID(std::string filename)
{
    if (textureMap.contains(filename))
    {
        return textureMap[filename];
    }
    else
    {
        return -1;
    }
}

float TextureManager::gigabytesLoadedToRAM()
{
    float bytes = 0;
    for (int i = 0; i < textures.size(); i++)
    {
        bytes += float(textures[i].width * textures[i].height * 4);
    }
    return bytes / (1024.0f * 1024.0f * 1024.0f);
}

float TextureManager::gigabytesLoadedToVRAM()
{
    float bytes = 0;
    for (int i = 0; i < textures.size(); i++)
    {
        if (textures[i].isResident)
        {
            bytes += float(textures[i].width * textures[i].height * 4);
        }
    }
    return bytes / (1024.0f * 1024.0f * 1024.0f);
}

Texture::Texture()
{
    width = 0;
    height = 0;
    nrChannels = 0;
    isResident = false;
    texData = nullptr;
    glGenTextures(1, &id);
}

Texture::Texture(std::string filename)
{
    width = 0;
    height = 0;
    nrChannels = 0;
    isResident = false;
    texData = nullptr;
    glGenTextures(1, &id);
    loadFromFile(filename);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
    if (texData)
    {
        stbi_image_free(texData);
    }
}

void Texture::loadToGPU()
{
    if (texData)
    {
        GLenum format;
        switch (nrChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            std::cout << "ERROR: Unsupported number of channels: " << nrChannels << std::endl;
            return;
        }
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        isResident = true;
    }
    else
    {
        std::cout << "ERROR: Texture missing, did loading fail?" << std::endl;
    }
}

void Texture::unloadFromGPU()
{
    glDeleteTextures(1, &id);
    glGenTextures(1, &id);
    isResident = false;
}

void Texture::loadFromFile(std::string filename)
{
    int width, height, nrChannels;
    texData = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (texData)
    {
        this->width = width;
        this->height = height;
        this->nrChannels = nrChannels;
    }
    else
    {
        std::cout << "ERROR: Failed to load texture" << std::endl;
    }
}
