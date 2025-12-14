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
    GLint baseSpecularColor;
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
    int debugBreak = 0;
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

    Shader lightSourceShader = Shader();
    lightSourceShader.compileVertexShader("shaders/lightSource_vert.glsl");
    lightSourceShader.compileFragmentShader("shaders/lightSource_frag.glsl");
    lightSourceShader.linkShaders();

    unsigned int texture1 = loadTexture("container2.png", "textures");
    unsigned int texture2 = loadTexture("container2_specular.png", "textures");
    unsigned int texture3 = loadTexture("awesomeface.png", "textures");
    unsigned int texture4 = loadTexture("jerma.jpg", "textures");


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

    std::cout << debugBreak++ << std::endl;
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

        // Send camera data to shaders
        lightSourceShader.use();
        setMat4(lightCamera, camera.transform);
        setVec3(lightColor, lightCol);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        meshShader.use();
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
