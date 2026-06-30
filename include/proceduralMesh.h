// Copyright (c) 2025 Kyle Bueche
// SPDX-License-Identifier: MIT
// Author: Kyle Bueche

#ifndef OPENGL_RENDERER_PROCEDURALMESH_H
#define OPENGL_RENDERER_PROCEDURALMESH_H

#include "mesh.h"

float lerp(float t, float a, float b);
Mesh triangle();
Mesh uvSphere(float radius, int uSegments, int vSegments);
Mesh uvPlane(float side, int uSegments, int vSegments);

#endif //OPENGL_RENDERER_PROCEDURALMESH_H