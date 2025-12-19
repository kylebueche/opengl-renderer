#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

Texture::Texture()
{
    glGenTextures(1, &id);
}

Texture::Texture(std::string filename)
{
    glGenTextures(1, &id);
    loadFromFile(filename);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void Texture::loadFromFile(std::string filename)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
        {
            format = GL_RED;
        }
        else if (nrChannels == 2)
        {
            format = GL_RG;
        }
        else if (nrChannels == 3)
        {
            format = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            format = GL_RGBA;
        }
        else
        {
            std::cout << "ERROR: Unsupported number of channels: " << nrChannels << std::endl;
            stbi_image_free(data);
            return;
        }
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        this->width = width;
        this->height = height;
        this->nrChannels = nrChannels;
    }
    else
    {
        std::cout << "ERROR: Failed to load texture" << std::endl;
    }
}
