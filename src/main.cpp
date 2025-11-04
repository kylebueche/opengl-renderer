#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <../thirdparty/stb_image.h>
#include <../thirdparty/PerlinNoise.hpp>

#include <iostream>
#include <cmath>
#include <numbers>
#include <random>
#include <chrono>

#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/texture.h"

struct particle
{
    glm::vec3 initialPosition;
    glm::vec3 initialVelocity;
};

class ParticleSystem
{
public:
    std::vector<particle> particles;
    std::vector<glm::vec3> positions;
    std::vector<uint32_t> meshIndices;
    std::vector<glm::vec3> meshVertices;
    std::vector<glm::vec3> meshNormals;
    std::vector<glm::vec2> meshTexCoords;
    GLuint VAO, VBOs[3], EBO;

    ParticleSystem(int numParticles)
    {
        meshVertices = {
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
        };
        meshNormals = meshVertices;
        meshTexCoords = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
        };
        meshIndices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            0, 4, 5, 5, 1, 0,
            0, 4, 7, 7, 3, 0,
            2, 6, 5, 5, 1, 2,
            2, 6, 7, 7, 3, 2
        };
        particles.resize(numParticles);
        positions.resize(numParticles);
        VAO = { 0 };
        VBOs[0] = { 0 };
        VBOs[1] = { 0 };
        VBOs[2] = { 0 };
        EBO = { 0 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(3, VBOs);
        glGenBuffers(1, &EBO);

        // Mesh VAO setup
        glBindVertexArray(VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * meshIndices.size(), meshIndices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshVertices.size(), meshVertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshNormals.size(), meshNormals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * meshTexCoords.size(), meshTexCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);
        glEnableVertexAttribArray(2);
        // Reset
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        for (int i = 0; i < particles.size(); i++)
        {
            float xDir = float(i % 7) / 7.0f - float(i % 3) / 3.0f + float(i % 23) / 23.0f - float(i % 13) / 13.0f;
            float zDir = float(i % 20) / 20.0f - float(i % 19) / 19.0f + float(i % 13) / 13.0f - float(i % 6) / 6.0f;
            particles[i].initialPosition = glm::vec3(5.0f, 0.0f, 5.0f);
            particles[i].initialVelocity = glm::vec3(5.0 * xDir, 15.0f, 5.0 * zDir);
        }
    }

    void interpolate(float t)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            positions[i] = particles[i].initialPosition + particles[i].initialVelocity * t - glm::vec3(0.0f, 0.5f * 9.81f, 0.0f) * t * t;
        }
    }

    void draw(GLint meshModel, GLint meshNormalMatrix)
    {
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < positions.size(); i++)
        {
            //float angle = 20.0f * i;
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, positions[i]);
            //transform = glm::rotate(transform, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            setMat4(meshModel, transform);
            setMat4(meshNormalMatrix, glm::mat3(glm::transpose(glm::inverse(transform))));
            glDrawElements(GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }

    ~ParticleSystem()
    {
        glDeleteBuffers(3, VBOs);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
};


struct PointLight
{
    glm::vec3 position;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float intensity;

    float quadratic;
    float linear;
    float constant;
};

struct PointLightUniform
{
    GLint position;
    GLint diffuse;
    GLint specular;
    GLint ambient;
    GLint intensity;

    GLint quadratic;
    GLint linear;
    GLint constant;
};

void setPointLight(PointLightUniform uniform, PointLight light)
{
    setVec3(uniform.position, light.position);
    setVec3(uniform.diffuse, light.diffuse);
    setVec3(uniform.specular, light.specular);
    setVec3(uniform.ambient, light.ambient);
    setFloat(uniform.intensity, light.intensity);

    setFloat(uniform.quadratic, light.quadratic);
    setFloat(uniform.linear, light.linear);
    setFloat(uniform.constant, light.constant);
}

struct SpotLight
{
    glm::vec3 position;
    glm::vec3 direction;
    float angle;
    float fadeAngle;

    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float intensity;

    float quadratic;
    float linear;
    float constant;
};

struct SpotLightUniform
{
    GLint position;
    GLint direction;
    GLint angle;
    GLint fadeAngle;

    GLint diffuse;
    GLint specular;
    GLint ambient;
    GLint intensity;

    GLint quadratic;
    GLint linear;
    GLint constant;
};

void setSpotLight(SpotLightUniform uniform, SpotLight light)
{
    setVec3(uniform.position, light.position);
    setVec3(uniform.direction, light.direction);
    setFloat(uniform.angle, light.angle);
    setFloat(uniform.fadeAngle, light.fadeAngle);

    setVec3(uniform.diffuse, light.diffuse);
    setVec3(uniform.specular, light.specular);
    setVec3(uniform.ambient, light.ambient);
    setFloat(uniform.intensity, light.intensity);

    setFloat(uniform.quadratic, light.quadratic);
    setFloat(uniform.linear, light.linear);
    setFloat(uniform.constant, light.constant);
}

struct DirLight
{
    glm::vec3 direction;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float intensity;
};

struct DirLightUniform
{
    GLint direction;
    GLint diffuse;
    GLint specular;
    GLint ambient;
    GLint intensity;
};

void setDirLight(DirLightUniform uniform, DirLight light)
{
    setVec3(uniform.direction, light.direction);
    setVec3(uniform.diffuse, light.diffuse);
    setVec3(uniform.specular, light.specular);
    setVec3(uniform.ambient, light.ambient);
    setFloat(uniform.intensity, light.intensity);
}

struct Material
{
    GLenum diffuse;
    GLenum specular;
    float shininess;
};

struct MaterialUniform
{
    GLint diffuse;
    GLint specular;
    GLint shininess;
};

void setMaterial(MaterialUniform uniform, Material mat)
{
    setInt(uniform.diffuse, mat.diffuse);
    setInt(uniform.specular, mat.specular);
    setFloat(uniform.shininess, mat.shininess);
}

// OpenGL functions
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Camera Settings
const char* windowTitle = "OpenGL Renderer";
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

PlayerCamera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

const siv::PerlinNoise::seed_type seed = 123456u;
const siv::PerlinNoise perlin{ seed };

int main()
{
    // GLFW Setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, windowTitle, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD Setup
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // STBI Settings
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader meshShader = Shader();
    meshShader.compileVertexShader("shaders/mesh_vert.glsl");
    meshShader.compileFragmentShader("shaders/mesh_frag.glsl");
    meshShader.linkShaders();

    Shader lightSourceShader = Shader();
    lightSourceShader.compileVertexShader("shaders/lightSource_vert.glsl");
    lightSourceShader.compileFragmentShader("shaders/lightSource_frag.glsl");
    lightSourceShader.linkShaders();

    //Shader waveShader = Shader();
    //waveShader.compileVertexShader("shaders/water_vert.glsl");
    //waveShader.compileFragmentShader("shaders/water_frag.glsl");
    //waveShader.linkShaders();

    unsigned int texture1 = attachTexture("textures/container2.png", GL_TEXTURE0);
    unsigned int texture2 = attachTexture("textures/container2_specular.png", GL_TEXTURE1);
    unsigned int texture3 = attachTexture("textures/awesomeface.png", GL_TEXTURE2);
    unsigned int texture4 = attachTexture("textures/jerma.jpg", GL_TEXTURE3);

    std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    const int numVerticesX = 1000;
    const int numVerticesZ = 1000;
    const int sizeX = 50;
    const int sizeZ = 50;
    const int layers = 3;
    const float frequency[layers] = { 0.07f, 0.2f, 0.5f };
    const float amplitude[layers] = { 10.0f, 2.5f, 0.5f };
    std::vector<glm::vec3> terrainVertices = std::vector<glm::vec3>(numVerticesX * numVerticesZ, glm::vec3(0.0f));
    std::vector<glm::vec3> terrainNormals = std::vector<glm::vec3>(numVerticesX * numVerticesZ, glm::vec3(0.0f));
    std::vector<glm::vec2> terrainTexCoords = std::vector<glm::vec2>(numVerticesX * numVerticesZ, glm::vec2(0.0f));
    std::vector<uint32_t> terrainIndices = std::vector<uint32_t>();

    int index, xIndex, yIndex, zIndex = 0;
    for (int i = 0; i < numVerticesX; i++)
    {
        for (int j = 0; j < numVerticesZ; j++)
        {
            index = (i * numVerticesX + j);

            terrainTexCoords[index] = glm::vec2(float(i) / float(numVerticesX - 1), float(j) / float(numVerticesZ - 1));

            float x = sizeX * (float(i) / numVerticesX - 0.5f);
            float z = sizeZ * (float(j) / numVerticesZ - 0.5f);
            float y = 0.0f;
            for (int h = 0; h < layers; h++)
            {
                y += amplitude[h] * perlin.noise2D(x * frequency[h], z * frequency[h]);
            }

            terrainVertices[index].x = x;
            terrainVertices[index].z = z;
            terrainVertices[index].y = y;

            terrainNormals[index].y = 1.0f;

            if (i > 0 && j > 0)
            {
                uint32_t topRight = i * numVerticesX + j;
                uint32_t topLeft = (i - 1) * numVerticesX + j;
                uint32_t bottomLeft = (i - 1) * numVerticesX + j - 1;
                uint32_t bottomRight = i * numVerticesX + j - 1;
                terrainIndices.push_back(topLeft);
                terrainIndices.push_back(bottomLeft);
                terrainIndices.push_back(bottomRight);
                terrainIndices.push_back(bottomRight);
                terrainIndices.push_back(topRight);
                terrainIndices.push_back(topLeft);

                glm::vec3 a, b;
                a = terrainVertices[topLeft] - terrainVertices[topRight];
                b = terrainVertices[topLeft] - terrainVertices[bottomLeft];
                terrainNormals[index] = glm::normalize(glm::cross(a, b));
            }
            
        }
    }

            

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VAOs[1] = { 0 };
    unsigned int VBOs[3] = { 0 };
    unsigned int EBOs[1] = { 0 };
    unsigned int lightVAO;
    unsigned int lightVBO;

    glGenVertexArrays(1, VAOs);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, EBOs);

    // Light VAO setup
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    
    // Mesh VAO setup
    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * terrainIndices.size(), terrainIndices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainVertices.size(), terrainVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainNormals.size(), terrainNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * terrainTexCoords.size(), terrainTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);
    glEnableVertexAttribArray(2);

    // Reset
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glm::vec3 blue = glm::vec3(0.0f, 0.1f, 0.9f);
    glm::vec3 white = glm::vec3(0.95f);
    glm::vec3 coral = glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 pos = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::mat4 lightTrans = glm::mat4(1.0f);
    lightTrans = glm::translate(lightTrans, pos);
    lightTrans = glm::scale(lightTrans, glm::vec3(0.5f));

    lightSourceShader.use();
    int lightTransform = lightSourceShader.getUniform("transform");
    int lightColor = lightSourceShader.getUniform("lightColor");
    int lightCamera = lightSourceShader.getUniform("camera");
    setMat4(lightTransform, lightTrans);
    setVec3(lightColor, white);
    setMat4(lightCamera, camera.transform);


    meshShader.use();
    int meshProjection = meshShader.getUniform("projection");
    int meshView = meshShader.getUniform("view");
    int meshModel = meshShader.getUniform("model");
    PointLightUniform pointLightUniform = {
        .position = meshShader.getUniform("pointLight.position"),
        .diffuse = meshShader.getUniform("pointLight.diffuse"),
        .specular = meshShader.getUniform("pointLight.specular"),
        .ambient = meshShader.getUniform("pointLight.ambient"),
        .intensity = meshShader.getUniform("pointLight.intensity"),
        .quadratic = meshShader.getUniform("pointLight.quadratic"),
        .linear = meshShader.getUniform("pointLight.linear"),
        .constant = meshShader.getUniform("pointLight.constant")
    };
    DirLightUniform dirLightUniform = {
        .direction = meshShader.getUniform("dirLight.direction"),
        .diffuse = meshShader.getUniform("dirLight.diffuse"),
        .specular = meshShader.getUniform("dirLight.specular"),
        .ambient = meshShader.getUniform("dirLight.ambient"),
        .intensity = meshShader.getUniform("dirLight.intensity"),
    };
    MaterialUniform materialUniform = {
        .diffuse = meshShader.getUniform("material.diffuse"),
        .specular = meshShader.getUniform("material.specular"),
        .shininess = meshShader.getUniform("material.shininess")
    };
    int meshNormalMatrix = meshShader.getUniform("normal");
    int meshViewPos = meshShader.getUniform("viewPos");

    PointLight pointLight = {
        .position = pos,
        .diffuse = blue,
        .specular = blue,
        .ambient = blue * 0.2f,
        .intensity = 100.0f,
        .quadratic = 1.0f,
        .linear = 1.0f,
        .constant = 1.0f
    };

    DirLight dirLight = {
        .direction = glm::vec3(0.2f, -1.0f, 0.1f),
        .diffuse = white,
        .specular = white,
        .ambient = white * 0.2f,
        .intensity = 0.1f,
    };

    Material material = {
        .diffuse = 0,
        .specular = 1,
        .shininess = 32.0f
    };
    setPointLight(pointLightUniform, pointLight);
    setDirLight(dirLightUniform, dirLight);
    setMaterial(materialUniform, material);

    setMat4(meshProjection, camera.projection);
    setMat4(meshView, camera.view);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
    setMat4(meshModel, model);
    
    camera.moveUp(20.0f);
    camera.updateView();
    camera.updateTransform();

    glm::vec3 cubePositions[] =
    {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    SpotLightUniform spotLightUniform = {
        .position = meshShader.getUniform("spotLight.position"),
        .direction = meshShader.getUniform("spotLight.direction"),
        .angle = meshShader.getUniform("spotLight.angle"),
        .fadeAngle = meshShader.getUniform("spotLight.fadeAngle"),
        .diffuse = meshShader.getUniform("spotLight.diffuse"),
        .specular = meshShader.getUniform("spotLight.specular"),
        .ambient = meshShader.getUniform("spotLight.ambient"),
        .intensity = meshShader.getUniform("spotLight.intensity"),
        .quadratic = meshShader.getUniform("spotLight.quadratic"),
        .linear = meshShader.getUniform("spotLight.linear"),
        .constant = meshShader.getUniform("spotLight.constant")
    };
    SpotLight spotLight = {
        .position = camera.position,
        .direction = camera.front,
        .angle = 17.0f,
        .fadeAngle = 20.0f,

        .diffuse = white,
        .specular = white,
        .ambient = 0.1f * white,
        .intensity = 1.0f,
        .quadratic = 0.001f,
        .linear = 0.001f,
        .constant = 1.0f
    };

    ParticleSystem particleSystem(100);

    lastFrame = glfwGetTime();
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // Inputs and updates
        currentFrame = glfwGetTime();
        glm::vec3 lightCol = glm::vec3(1.0f);
        //lightCol.x = sin(currentFrame * 1.9f + 6.28f / 3.0f);
        //lightCol.y = sin(currentFrame * 2.3f + 6.28f * 2.0f / 3.0f);
        //lightCol.z = sin(currentFrame * 1.73f);
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        static float currentTime = 0.0f;
        currentTime += deltaTime;
        particleSystem.interpolate(currentTime);
        if (currentTime > 10.0f) { currentTime = 0.0f; }

        // Canvas reset
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Send camera data to shaders
        lightSourceShader.use();
        glBindVertexArray(lightVAO);
        setMat4(lightCamera, camera.transform);
        setVec3(lightColor, lightCol);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        meshShader.use();
        glBindVertexArray(VAOs[0]);
        spotLight.position += (camera.position - spotLight.position) * deltaTime * 5.0f;
        spotLight.direction += (camera.front - spotLight.direction) * deltaTime * 5.0f;
        setSpotLight(spotLightUniform, spotLight);
        setVec3(meshViewPos, camera.position);
        setMat4(meshProjection, camera.projection);
        setMat4(meshView, camera.view);
        setPointLight(pointLightUniform, pointLight);
        setDirLight(dirLightUniform, dirLight);
        setMaterial(materialUniform, material);
        setMat4(meshModel, glm::mat4(1.0f));
        setMat4(meshNormalMatrix, glm::mat3(glm::transpose(glm::inverse(glm::mat4(1.0f)))));
        glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        particleSystem.draw(meshModel, meshNormalMatrix);

        /*
        for (unsigned int i = 0; i < 10; i++)
        {
            float angle = 20.0f * i;
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, cubePositions[i]);
            transform = glm::rotate(transform, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            setMat4(meshModel, transform);
            setMat4(meshNormalMatrix, glm::mat3(glm::transpose(glm::inverse(transform))));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        */

        // Swap frame buffers and get next events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, VAOs);
    glDeleteBuffers(3, VBOs);
    glDeleteBuffers(1, EBOs);

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);

    glfwTerminate();

    return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    static bool firstMotion = true;
    static float lastX, lastY;

    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
    {
        firstMotion = true;
        return;
    }

    if (firstMotion)
    {
        lastX = xPos;
        lastY = yPos;
        firstMotion = false;
    }
    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    const float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera.rotateBy(xOffset, yOffset);
    camera.updateView();
    camera.updateTransform();
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.zoomBy((float) yOffset);
    camera.updateProjection();
    camera.updateTransform();
}

// OpenGL functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    float distance = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.moveForward(distance);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.moveBackward(distance);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.moveLeft(distance);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.moveRight(distance);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.moveUp(distance);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.moveDown(distance);
    }
    camera.updateView();
    camera.updateTransform();
    
}
