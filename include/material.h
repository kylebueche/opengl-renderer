//
// Created by kyleb on 11/1/2025.
//

#ifndef OPENGL_RENDERER_MATERIAL_H
#define OPENGL_RENDERER_MATERIAL_H


#include "glad/glad.h"
#include <vector>
#include "texture.h"

#include "glm/vec3.hpp"

struct Material;

struct MaterialManager
{
    std::vector<Material> materials;
    std::unordered_map<std::string, Material> materialsMap;
};


struct Material
{
    glm::vec3 baseDiffuseColor;
    glm::vec3 baseSpecularColor;
    std::vector<Texture> diffuseTextures;
    std::vector<Texture> specularTextures;
    float shininess;

    Material();
    Material(glm::vec3 diffuse);
    Material(glm::vec3 diffuse, glm::vec3 specular);
    Material(glm::vec3 diffuse, glm::vec3 specular, float shininess);
};

#endif //OPENGL_RENDERER_MATERIAL_H