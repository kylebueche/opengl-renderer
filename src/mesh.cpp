#include "mesh.h"
#include <cmath>
#include <numbers>
#include "glm/ext/matrix_transform.hpp"

Mesh::Mesh()
{
    VAO = 0;
    VBOs[0] = 0;
    VBOs[1] = 0;
    VBOs[2] = 0;
    EBO = 0;
    vertices = std::vector<glm::vec3>();
    normals = std::vector<glm::vec3>();
    texCoords = std::vector<glm::vec2>();
    indices = std::vector<unsigned int>();
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    model = glm::mat4(1.0f);
    normal = glm::mat3(1.0f);
}

Mesh::Mesh(const Mesh& other)
{
    VAO = 0;
    VBOs[0] = 0;
    VBOs[1] = 0;
    VBOs[2] = 0;
    EBO = 0;
    vertices = other.vertices;
    normals = other.normals;
    texCoords = other.texCoords;
    indices = other.indices;
    position = other.position;
    rotation = other.rotation;
    scale = other.scale;
    model = other.model;
    normal = other.normal;
}

Mesh::~Mesh()
{
    deleteBuffersIfAllocated();
}

void Mesh::bufferToGPU()
{
    deleteBuffersIfAllocated();
    allocateBuffers();

    // Vertex Array Object
    glBindVertexArray(VAO);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    glEnableVertexAttribArray(0);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    glEnableVertexAttribArray(1);

    // Texture Coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);
    glEnableVertexAttribArray(2);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::calculateModel()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
}

void Mesh::calculateNormal()
{
    normal = glm::mat3(glm::transpose(glm::inverse(model)));
}

void Mesh::allocateBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);
}

void Mesh::deleteBuffersIfAllocated()
{
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
    for (int i = 0; i < 3; i++)
        if (VBOs[i] != 0)
            glDeleteBuffers(1, &VBOs[i]);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

