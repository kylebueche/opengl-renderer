//
// Created by kyleb on 12/12/2025.
//

#include "lights.h"

DirLight::DirLight() :
    direction(glm::vec3(0.0f, -1.0f, 0.0f)),
    intensity(1.0f),
    ambient(glm::vec3(0.1f)),
    specular(glm::vec3(1.0f)),
    diffuse(glm::vec3(1.0f))
{

}


DirLight::DirLight(glm::vec3 color) :
    direction(glm::vec3(0.0f, -1.0f, 0.0f)),
    intensity(1.0f),
    ambient(0.1f * color),
    specular(color),
    diffuse(color)
{

}


PointLight::PointLight() :
    position(glm::vec3(0.0f, 1.0f, 0.0f)),
    intensity(1.0f),
    ambient(glm::vec3(0.1f)),
    specular(glm::vec3(1.0f)),
    diffuse(glm::vec3(1.0f)),
    constant(1.0f),
    linear(0.0f),
    quadratic(1.0f)
{

}


PointLight::PointLight(glm::vec3 color) :
    position(glm::vec3(0.0f, 1.0f, 0.0f)),
    intensity(1.0f),
    ambient(0.1f * color),
    specular(color),
    diffuse(color),
    constant(1.0f),
    linear(0.0f),
    quadratic(1.0f)
{

}


SpotLight::SpotLight() :
    position(glm::vec3(0.0f, 1.0f, 0.0f)),
    angle(20.0f),
    fadeAngle(30.0f),
    intensity(1.0f),
    ambient(glm::vec3(0.1f)),
    specular(glm::vec3(1.0f)),
    diffuse(glm::vec3(1.0f)),
    constant(1.0f),
    linear(0.0f),
    quadratic(1.0f)
{

}


SpotLight::SpotLight(glm::vec3 color) :
    position(glm::vec3(0.0f, 1.0f, 0.0f)),
    angle(20.0f),
    fadeAngle(30.0f),
    intensity(1.0f),
    ambient(0.1f * color),
    specular(color),
    diffuse(color),
    constant(1.0f),
    linear(0.0f),
    quadratic(1.0f)
{

}