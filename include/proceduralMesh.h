//
// Created by kyleb on 12/18/2025.
//

#ifndef OPENGL_RENDERER_PROCEDURALMESH_H
#define OPENGL_RENDERER_PROCEDURALMESH_H

#include "mesh.h"

float lerp(float t, float a, float b);
Mesh triangle();
Mesh uvSphere(float radius, int uSegments, int vSegments);
Mesh uvPlane(float side, int uSegments, int vSegments);

#endif //OPENGL_RENDERER_PROCEDURALMESH_H