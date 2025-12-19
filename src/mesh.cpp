#include "mesh.h"
#include <cmath>
#include <numbers>
#include "glm/ext/matrix_transform.hpp"

Mesh::Mesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);
    vertices = std::vector<glm::vec3>();
    normals = std::vector<glm::vec3>();
    texCoords = std::vector<glm::vec2>();
    indices = std::vector<unsigned int>();
}

Mesh::Mesh(const Mesh& other)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);
    vertices = other.vertices;
    normals = other.normals;
    texCoords = other.texCoords;
    indices = other.indices;
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(3, VBOs);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::bufferToGPU()
{
    // Vertex Array Object
    glBindVertexArray(VAO);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

    // Texture Coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind Vertex Array
    glBindVertexArray(0);
}

void Mesh::draw()
{
    // Bind pre-buffered data on the GPU side
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
