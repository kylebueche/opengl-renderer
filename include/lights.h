//
// Created by kyleb on 12/12/2025.
//

#ifndef OPENGL_RENDERER_LIGHTS_H
#define OPENGL_RENDERER_LIGHTS_H
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <string>

struct DirLight {
    glm::vec3 direction;
    float intensity;

    // Color
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirLight();
    DirLight(glm::vec3 color);
};

struct PointLight {
    glm::vec3 position;
    float intensity;

    // Color
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // Attenuation
    float quadratic;
    float linear;
    float constant;

    PointLight();
    PointLight(glm::vec3 color);
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float angle;
    float fadeAngle;
    float intensity;

    // Color
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // Attenuation
    float quadratic;
    float linear;
    float constant;

    SpotLight();
    SpotLight(glm::vec3 color);
};


#endif //OPENGL_RENDERER_LIGHTS_H