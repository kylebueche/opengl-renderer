#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

GLint attachTexture(const char* texturePath, GLenum glTexture)
{
    unsigned int texture;
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    glGenTextures(1, &texture);
    glActiveTexture(glTexture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "ERROR: Unsupported number of channels: " << nrChannels << std::endl;
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "ERROR: Failed to load texture" << std::endl;
    }
    return texture;
}

