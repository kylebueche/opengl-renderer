// Copyright (c) 2025 Kyle Bueche
// SPDX-License-Identifier: MIT
// Author: Kyle Bueche

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