#ifndef MESH_H
#define MESH_H

#include <cmath>
#include <numbers>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "texture.h"
#include "material.h"

class Mesh
{
public:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 model;
        glm::mat3 normal;
        // VBO Data
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        // EBO Data
        std::vector<unsigned int> indices;

        Mesh();
        Mesh(const Mesh& other);
        ~Mesh();
        void calculateModel();
        void calculateNormal();

        void bufferToGPU();
        void draw();

private:
        GLuint VAO;
        GLuint VBOs[3];
        GLuint EBO;
        void allocateBuffers();
        void deleteBuffersIfAllocated();
};

Mesh makeTriangle();


Mesh makefibonnacciSphere(int samples);
#endif
