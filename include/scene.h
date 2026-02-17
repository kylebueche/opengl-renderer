//
// Created by Kyle on 2/16/2026.
//

#ifndef OPENGL_RENDERER_SCENE_H
#define OPENGL_RENDERER_SCENE_H

#include "lights.h"
#include "camera.h"
#include "material.h"

struct scene
{
    std::vector<PointLight> pointLights;
    std::vector<DirLight> dirLights;
    std::vector<SpotLight> spotLights;
    Camera camera;

private:
    Material material;
};

#endif //OPENGL_RENDERER_SCENE_H