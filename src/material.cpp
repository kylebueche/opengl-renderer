//
// Created by kyleb on 12/13/2025.
//

#include "material.h"


Material::Material()
    : baseDiffuseColor(glm::vec3(1.0f)),
      baseSpecularColor(glm::vec3(0.0f)),
      shininess(0.0f)
{}

Material::Material(glm::vec3 diffuse)
    : baseDiffuseColor(diffuse),
      baseSpecularColor(glm::vec3(0.0f)),
      shininess(0.0f)
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