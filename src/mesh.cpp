#include "mesh.h"

#include <cmath>
#include <numbers>

Mesh::Mesh()
{
    vertices = NULL;
    faces = NULL;
}

Mesh::Mesh(int numPoints)
{
    vertices = new float[3 * numPoints];
    faces = NULL;
}

Mesh::Mesh(int numPoints, int numFaces)
{
    vertices = new float[3 * numPoints];
    faces = new float[3 * numFaces];
}

Mesh::~Mesh()
{
    if (vertices != NULL)
    {
        delete(vertices);
    }
    if (faces != NULL)
    {
        delete(faces);
    }
}

Mesh fibonnacciSphere(int samples)
{
    Mesh mesh = Mesh(samples);
    float goldenAngle = std::numbers::pi * (sqrt(5.0f) - 1.0f);
    float x, y, z = 0;
    float radius = 0;
    float theta = 0;

    for (int i = 0; i < samples; i++)
    {
        y = 1 - (i / float(samples - 1)) * 2;
        radius = sqrt(1 - y * y);
        
        theta = goldenAngle * i;

        x = cos(theta) * radius;
        z = sin(theta) * radius;

        mesh.vertices[i] = x;
        mesh.vertices[i + 1] = y;
        mesh.vertices[i + 2] = z;
    }
    return mesh;
}

Mesh triangle()
{
    Mesh mesh = Mesh(3, 1);
    mesh.vertices[0] = -0.5f;
    mesh.vertices[1] = -0.5f;
    mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f;
    mesh.vertices[4] = -0.5f;
    mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = 0.0f;
    mesh.vertices[7] = 1.0f;
    mesh.vertices[8] = 0.0f;
    mesh.faces[0] = 0;
    mesh.faces[1] = 3;
    mesh.faces[2] = 6;
    return mesh;
}
