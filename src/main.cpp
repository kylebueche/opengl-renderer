#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <../thirdparty/include/stb_image.h>
#include <../thirdparty/include/PerlinNoise.hpp>

#include <iostream>
#include <cmath>
#include <numbers>
#include <random>
#include <chrono>

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "mesh.h"
#include "proceduralMesh.h"

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

Camera camera;

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
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Shader meshShader = Shader();
    meshShader.compileVertexShader("shaders/mesh_vert.glsl");
    meshShader.compileFragmentShader("shaders/mesh_frag.glsl");
    meshShader.linkShaders();

    Texture texture("textures/container2.png");

    meshShader.use();
    MeshUniform meshUniform = meshShader.getMeshUniform("mesh");
    CameraUniform cameraUniform = meshShader.getCameraUniform("camera");
    SpotLightUniform spotLightUniform = meshShader.getSpotLightUniform("spotLights[0]");
    PointLightUniform pointLightUniform = meshShader.getPointLightUniform("pointLight[0]");
    DirLightUniform dirLightUniform = meshShader.getDirLightUniform("dirLight[0]");
    GLint numSpotLightsUniform = meshShader.getUniform("numSpotLights");
    GLint numPointLightsUniform = meshShader.getUniform("numPointLights");
    GLint numDirLightsUniform = meshShader.getUniform("numDirLights");
    setInt(numSpotLightsUniform, 1);
    setInt(numPointLightsUniform, 1);
    setInt(numDirLightsUniform, 1);

    MaterialUniform materialUniform = meshShader.getMaterialUniform("material");

    // Colors
    glm::vec3 white = glm::vec3(1, 1, 1);
    glm::vec3 red = glm::vec3(1, 0, 0);
    glm::vec3 orange = glm::vec3(1, 1, 0);
    glm::vec3 yellow = glm::vec3(0, 1, 1);
    glm::vec3 green = glm::vec3(0, 1, 0);
    glm::vec3 blue = glm::vec3(0, 0, 1);
    glm::vec3 purple = glm::vec3(1, 0, 1);


    PointLight pointLight = {
        .position = glm::vec3(1.0f, 1.0f, 1.0f),
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

    camera.updateProjection();
    camera.updateView();

    Mesh sphere = uvSphere(1.0f, 10.0f, 10.0f);
    sphere.position = glm::vec3(0.0f, 0.0f, 10.0f);
    sphere.bufferToGPU();
    sphere.calculateModel();
    sphere.calculateNormal();

    meshShader.use();
    Material material(red);
    setSpotLight(spotLightUniform, spotLight);
    setPointLight(pointLightUniform, pointLight);
    setDirLight(dirLightUniform, dirLight);
    setMaterial(materialUniform, material);
    setMesh(meshUniform, sphere);
    setCamera(cameraUniform, camera);

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
        if (currentTime > 10.0f) { currentTime = 0.0f; }

        // Canvas reset
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        spotLight.position += (camera.position - spotLight.position) * deltaTime * 5.0f;
        spotLight.direction += (camera.front - spotLight.direction) * deltaTime * 5.0f;
        camera.updateProjection();
        camera.updateView();
        sphere.calculateModel();
        sphere.calculateNormal();
        meshShader.use();
        setSpotLight(spotLightUniform, spotLight);
        setPointLight(pointLightUniform, pointLight);
        setDirLight(dirLightUniform, dirLight);
        setMaterial(materialUniform, material);
        setMesh(meshUniform, sphere);
        setCamera(cameraUniform, camera);
        sphere.draw();

        // Swap frame buffers and get next events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.zoomBy((float) yOffset);
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
    
}
