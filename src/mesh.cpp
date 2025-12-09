#include "mesh.h"
#include <cmath>
#include <numbers>

Mesh::Mesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);
    vertices = std::vector<glm::vec3>();
    normals = std::vector<glm::vec3>();
    texCoords = std::vector<glm::vec2>();
    indices = std::vector<unsigned int>();
    textures = std::vector<Texture>();
    bufferToGPU();
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
    bufferToGPU();
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

Mesh triangle()
{
    Mesh mesh;
    mesh.vertices =
    {
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    };
    mesh.normals =
    {
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    mesh.indices = { 0, 1, 2 };
    return mesh;
}

void Mesh::draw(Shader& shader)
{
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr);
            diffuseNr++;
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specularNr);
            specularNr++;
        }
        GLint uniform = shader.getUniform(("material." + name + number).c_str());
        setInt(uniform, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(textures.size());
    glBindTexture(GL_TEXTURE_2D, defaultTexture.id);
    for (int i = diffuseNr; i <= 8; i++)
    {
        GLint uniform = shader.getUniform(("material.texture_diffuse" + std::to_string(i)).c_str());
        setInt(uniform, textures.size());
    }
    for (int i = specularNr; i <= 8; i++)
    {
        GLint uniform = shader.getUniform(("material.specular_diffuse" + std::to_string(i)).c_str());
        setInt(uniform, textures.size());
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

Texture Mesh::defaultTexture = Texture(glm::vec3(1.0f));