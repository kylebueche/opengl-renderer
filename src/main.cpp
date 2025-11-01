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

    Shader waveShader = Shader();
    waveShader.compileVertexShader("shaders/water_vert.glsl");
    waveShader.compileFragmentShader("shaders/water_frag.glsl");
    waveShader.linkShaders();

    unsigned int texture1 = attachTexture("textures/jerma.jpg", GL_TEXTURE0);
    unsigned int texture2 = attachTexture("textures/awesomeface.png", GL_TEXTURE1);

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
    std::vector<uint32_t> terrainIndices = std::vector<uint32_t>();

    int index, xIndex, yIndex, zIndex = 0;
    for (int i = 0; i < numVerticesX; i++)
    {
        for (int j = 0; j < numVerticesZ; j++)
        {
            index = (i * numVerticesX + j);

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
    unsigned int VBOs[2] = { 0 };
    unsigned int EBOs[1] = { 0 };
    unsigned int lightVAO;
    unsigned int lightVBO;

    glGenVertexArrays(1, VAOs);
    glGenBuffers(2, VBOs);
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

    // Reset
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glm::vec3 white = glm::vec3(0.95f);
    glm::vec3 coral = glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 pos = glm::vec3(0.0f, 10.0f, 0.0f);
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
    int meshAmbient = meshShader.getUniform("material.ambient");
    int meshDiffuse = meshShader.getUniform("material.diffuse");
    int meshSpecular = meshShader.getUniform("material.specular");
    int meshShininess = meshShader.getUniform("material.shininess");
    int meshLightAmbient = meshShader.getUniform("light.ambient");
    int meshLightDiffuse = meshShader.getUniform("light.diffuse");
    int meshLightSpecular = meshShader.getUniform("light.specular");
    int meshLightPos = meshShader.getUniform("light.position");
    int meshNormalMatrix = meshShader.getUniform("normal");
    int meshViewPos = meshShader.getUniform("viewPos");
    setVec3(meshAmbient, coral);
    setVec3(meshDiffuse, coral);
    setVec3(meshSpecular, coral);
    setFloat(meshShininess, 32.0f);
    setVec3(meshLightAmbient, 0.2f * white);
    setVec3(meshLightDiffuse, 0.5f * white);
    setVec3(meshLightSpecular, 1.0f * white);
    setVec3(meshLightPos, pos);
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


    lastFrame = glfwGetTime();
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // Inputs and updates
        currentFrame = glfwGetTime();
        glm::vec3 lightCol;
        lightCol.x = sin(currentFrame * 1.9f + 6.28f / 3.0f);
        lightCol.y = sin(currentFrame * 2.3f + 6.28f * 2.0f / 3.0f);
        lightCol.z = sin(currentFrame * 1.73f);
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

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
        setVec3(meshViewPos, camera.position);
        setMat4(meshProjection, camera.projection);
        setMat4(meshView, camera.view);
        setVec3(meshAmbient, 0.8f * coral);
        setVec3(meshDiffuse, coral);
        setVec3(meshSpecular, 0.3f * coral);
        setFloat(meshShininess, 32.0f);

        setVec3(meshLightAmbient, 0.2f * lightCol);
        setVec3(meshLightDiffuse, 0.5f * lightCol);
        setVec3(meshLightSpecular, 1.0f * lightCol);
        setVec3(meshLightPos, pos);
        setMat4(meshModel, glm::mat4(1.0f));
        setMat4(meshNormalMatrix, glm::mat3(glm::transpose(glm::inverse(glm::mat4(1.0f)))));
        glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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
    glDeleteBuffers(2, VBOs);
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
