//
// Created by kyleb on 12/13/2025.
//

#include "material.h"


Material::Material()
    : baseDiffuseColor(white),
      baseSpecularColor(black),
      diffuseTextures(std::vector<Texture>()),
      specularTextures(std::vector<Texture>()),
      shininess(1.0f)
{}

Material::Material(glm::vec3 diffuse)
    : baseDiffuseColor(diffuse),
      baseSpecularColor(black),
      shininess(1.0f)
{}

Material::Material(glm::vec3 diffuse, glm::vec3 specular)
    : baseDiffuseColor(diffuse),
      baseSpecularColor(specular),
      shininess(32.0f)
{}

Material::Material(glm::vec3 diffuse, glm::vec3 specular, float shininess)
    : baseDiffuseColor(diffuse),
      baseSpecularColor(specular),
      shininess(shininess)
{}


Material::Material(std::string diffuseTex)
    :   baseDiffuseColor(black),
        baseSpecularColor(black),
        diffuseTextures({Texture(diffuseTex)}),
        shininess(1.0f)
{}

Material::Material(std::string diffuseTex, std::string specularTex)
    :   baseDiffuseColor(black),
        baseSpecularColor(black),
        diffuseTextures({Texture(diffuseTex)}),
        specularTextures({Texture(specularTex)}),
        shininess(1.0f)
{}