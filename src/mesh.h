#ifndef MESH_H
#define MESH_H

#include <cmath>
#include <numbers>

class Mesh
{
    public:
        float *vertices;
        float *faces;

        Mesh();
        Mesh(int numPoints);
        Mesh(int numPoints, int numFaces);
        ~Mesh();
};

Mesh makefibonnacciSphere(int samples);
Mesh makeTriangle();

#endif
