#ifndef MESH_H
#define MESH_H

#include <cmath>
#include <numbers>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "shader.h"
#include "texture.h"

class Mesh
{
public:
        glm::mat4 transform;
        // VBO Data
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        // EBO Data
        std::vector<unsigned int> indices;
        // Texture Data
        std::vector<Texture> textures;

        Mesh();
        Mesh(const Mesh& other);
        ~Mesh();
        void bufferToGPU();
        void draw(Shader &shader);

private:
        GLuint VAO;
        GLuint VBOs[3];
        GLuint EBO;
        static Texture defaultTexture;
};

Mesh makefibonnacciSphere(int samples);
Mesh makeTriangle();

#endif
