//
// Created by kyleb on 12/8/2025.
//
#include <glm/vec3.hpp>
#include <vector>
#include <cmath>

#include "mesh.h"
#include "glm/mat4x4.hpp"
#include "glm/geometric.hpp"
#include "glm/ext/matrix_transform.hpp"

float lerp(float t, float a, float b)
{
    return t * (b - a) + a;
}

Mesh triangle()
{
    Mesh triangle;
    triangle.vertices = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
    };
    triangle.normals = {
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
    };
    triangle.texCoords = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    };
    triangle.indices = {
        0, 1, 2,
    };
    return triangle;
}

Mesh uvPlane(float side, int uSegments, int vSegments)
{
    Mesh plane;
    for (unsigned int u = 0; u < uSegments; u++)
    {
        for (unsigned int v = 0; v < vSegments; v++)
        {
            float u_01 = float(u) / float(uSegments);
            float v_01 = float(v) / float(vSegments);
            float xVal = lerp(u_01, -side / 2.0f, +side / 2.0f);
            float zVal = lerp(v_01, -side / 2.0f, +side / 2.0f);
            plane.vertices.push_back(glm::vec3(xVal, 0.0f, zVal));
            plane.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
            plane.texCoords.push_back(glm::vec2(u_01, v_01));
        }
    }
    for (unsigned int u = 0; u + 1 < uSegments; u++)
    {
        for (unsigned int v = 0; v + 1 < vSegments; v++)
        {
            unsigned int topLeft = u * vSegments + v;
            unsigned int topRight = u * vSegments + v + 1;
            unsigned int bottomLeft = (u + 1) * vSegments + v;
            unsigned int bottomRight = (u + 1) * vSegments + v + 1;
            plane.indices.push_back(topLeft);
            plane.indices.push_back(topRight);
            plane.indices.push_back(bottomLeft);
            plane.indices.push_back(bottomRight);
            plane.indices.push_back(bottomLeft);
            plane.indices.push_back(topRight);
        }
    }
    return plane;
}

Mesh uvSphere(float radius, int uSegments, int vSegments)
{
    Mesh sphere;

    if (uSegments < 3)
        uSegments = 3;
    if (vSegments < 2)
        vSegments = 2;

    float deltaV = M_PI / vSegments;
    float deltaU = 2 * M_PI / uSegments;
    float angleV;
    float angleU;
    for (int i = 0; i <= vSegments; i++)
    {
        angleV = M_PI / 2.0 - i * deltaV;
        float xy = radius * cosf(angleV);
        float z = radius * sinf(angleV);
        for (int j = 0; j <= uSegments; j++)
        {
            angleU = j * deltaU;
            glm::vec3 point = glm::vec3(xy * cosf(angleU), xy * sinf(angleU), z);
            sphere.vertices.push_back(point);
            sphere.normals.push_back(glm::normalize(point));
            sphere.texCoords.push_back(glm::vec2(float(j) / uSegments, float(i) / vSegments));
        }
    }

    unsigned int k1, k2;
    for (int i = 0; i < vSegments; i++)
    {
        k1 = i * (uSegments + 1);
        k2 = k1 + uSegments + 1;
        for (int j = 0; j < uSegments; j++, k1++, k2++)
        {
            if (i != 0)
            {
                sphere.indices.push_back(k1);
                sphere.indices.push_back(k2);
                sphere.indices.push_back(k1 + 1);
            }
            if (i != (uSegments - 1))
            {
                sphere.indices.push_back(k1 + 1);
                sphere.indices.push_back(k2);
                sphere.indices.push_back(k2 + 1);
            }
        }
    }
    return sphere;
}

/*
// Efficiently stores heightfield data given
class HeightField
{
public:
    glm::vec3 position_00;
    glm::vec3 u_unit_vector;
    glm::vec3 v_unit_vector;
    glm::vec3 u_side_vector;
    glm::vec3 v_side_vector;
    glm::vec3 u_to_next;
    glm::vec3 v_to_next;
    float u_length;
    float v_length;
    std::vector<float> heights;
    std::vector<uint32_t> indices;

    HeightField(int sizeX, int sizeZ, float width, float length, float (*heightFunc)(float, float))
    {
        u_side_vector = glm::vec3(width, 0.0f, 0.0f);
        v_side_vector = glm::vec3(0.0f, 0.0f, length);
        u_to_next = u_side_vector / float(sizeX);
        v_to_next = v_side_vector / float(sizeZ);
        u_unit_vector glm::vec3(1.0f, 0.0f, 0.0f);
        v_unit_vector glm::vec3(0.0f, 0.0f, 1.0f);

        float x, z;
        for (int i = 0; i < sizeX; i++)
        {
            x = lerp(i / sizeX - 1, 0.0f, width);
            for (int j = 0; j < sizeZ; j++)
            {
                z = lerp(j / (sizeZ - 1), 0.0f, length);
                float y = heightFunc(x, z);
                heights.push_back(y);
                if (i > 0 && j > 0)
                {
                    uint32_t topRight = i * sizeX + j;
                    uint32_t topLeft = (i - 1) * sizeX + j;
                    uint32_t bottomLeft = (i - 1) * sizeX + j - 1;
                    uint32_t bottomRight = i * sizeX + j - 1;
                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);
                    indices.push_back(bottomRight);
                    indices.push_back(topRight);
                    indices.push_back(topLeft);
                }
            }
        }
    }
};
*/