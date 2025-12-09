#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

GLint loadTexture(const char* texturePath, std::string directory)
{
    std::string filename = directory + "/" + std::string(texturePath);

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
        if (nrChannels == 3)
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
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "ERROR: Failed to load texture" << std::endl;
    }
    return textureID;
}

GLint colorTexture(glm::vec3 color)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width = 2;
    int height = 2;
    glm::vec3 colors[] = {color, color, color, color};
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, colors);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(glm::vec3 color)
{
    id = colorTexture(color);
    type = "color";
    path = "procedural";
}

Texture::Texture(GLuint id, std::string type, std::string path) : id(id), type(type), path(path) {}