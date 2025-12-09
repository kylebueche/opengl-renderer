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

Mesh Sphere(float radius, float uSegments, int vSegments)
{
    Mesh sphere;
    glm::vec3 point = glm::vec3(radius, 0.0f, 0.0f);
    for (int i = 0; i < uSegments; i++)
    {
        for (int j = 0; j < vSegments; j++)
        {
            float angleU = lerp(float(i) / uSegments, 0.0f, 2.0f * M_PI);
            float angleV = lerp(float(j) / vSegments, -M_PI / 2.0f, M_PI / 2.0f);
            glm::mat4 mat(1.0f);
            // Rotate up and down first
            mat = glm::rotate(mat, angleV, glm::vec3(0.0f, 0.0f, 1.0f));
            // Rotate around vertical axis last
            mat = glm::rotate(mat, angleU, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec3 newPoint = mat * point;
            sphere.vertices.push_back(newPoint);
            sphere.normals.push_back(normalize(newPoint));
            sphere.texCoords.push_back(glm::vec2(angleU, angleV));
        }
    }
}

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