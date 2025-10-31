#ifndef MESH_H
#define MESH_H

#include <cmath>
#include <numbers>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Mesh
{
    public:
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<int> indices;
        glm::vec3 color;

        Mesh();
        ~Mesh();
        void bufferToGPU();
        void draw();

};

Mesh makefibonnacciSphere(int samples);
Mesh makeTriangle();

#endif
