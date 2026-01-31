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
#include "lights.h"

// OpenGL functions
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void GLAPIENTRY debug_message_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam);


// Global Camera Settings
const char* windowTitle = "OpenGL Renderer";
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

#define E_CUOLOMBS 1.6021766343e-19
#define CUOLOMBS_CONSTANT_NC2_PER_M2 8.987551785972e9
#define ELECTRON_MASS_KG 9.1093837139e-31
#define PROTON_MASS_KG 1.67262192595e-27
#define ELECTRON_RADIUS 1.0e-22 // this is not a true radius, but a functional size indication
#define PROTON_RADIUS 0.84075e-15
#define HYDROGEN_ORBIT_RADIUS 5.29e-11

// acceleration per distance squared between the two particles.
// these are all surprisingly tame values between 0.01 and 300.0
constexpr float accel_on_e_from_p_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * -E_CUOLOMBS * E_CUOLOMBS / ELECTRON_MASS_KG;
constexpr float accel_on_p_from_e_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * -E_CUOLOMBS * E_CUOLOMBS / PROTON_MASS_KG;
constexpr float accel_on_e_from_e_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * E_CUOLOMBS * E_CUOLOMBS / ELECTRON_MASS_KG;
constexpr float accel_on_p_from_p_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * E_CUOLOMBS * E_CUOLOMBS / PROTON_MASS_KG;

int main()
{
    // GLFW Setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
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
    glDebugMessageCallback(debug_message_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // STBI Settings
    stbi_set_flip_vertically_on_load(true);

    Shader meshShader = Shader();
    meshShader.compileVertexShader("shaders/mesh_vert.glsl");
    meshShader.compileFragmentShader("shaders/mesh_frag.glsl");
    meshShader.linkShaders();

    meshShader.use();
    MeshUniform meshUniform = meshShader.getMeshUniform("mesh");
    MaterialUniform materialUniform = meshShader.getMaterialUniform("material");
    CameraUniform cameraUniform = meshShader.getCameraUniform("camera");
    SpotLightUniform spotLightUniform = meshShader.getSpotLightUniform("spotLights[0]");
    PointLightUniform pointLightUniform = meshShader.getPointLightUniform("pointLights[0]");
    DirLightUniform dirLightUniform = meshShader.getDirLightUniform("dirLights[0]");
    GLint numSpotLightsUniform = meshShader.getUniform("numSpotLights");
    GLint numPointLightsUniform = meshShader.getUniform("numPointLights");
    GLint numDirLightsUniform = meshShader.getUniform("numDirLights");
    setInt(numSpotLightsUniform, 1);
    setInt(numPointLightsUniform, 1);
    setInt(numDirLightsUniform, 1);

    glm::vec3 red = glm::vec3(1, 0, 0.3f);
    Material material(red);

    PointLight pointLight;
    pointLight.intensity = 50.0f;

    DirLight dirLight;
    dirLight.intensity = 1.0f;
    dirLight.direction = glm::vec3(-0.5f, -1.0f, -0.5f);
    SpotLight spotLight;

    spotLight.position = camera.position;
    spotLight.direction = camera.front;
    spotLight.angle =  17.0f;
    spotLight.fadeAngle = 20.0f;

    Mesh mesh = uvSphere(2.0, 10, 10);
    mesh.position = glm::vec3(0.0f, -1.0f, 10.0f);
    camera.position = glm::vec3(0.0f, 0.0f, 10.0f);

    Mesh proton = uvSphere(0.1f, 10, 10);
    Mesh electron = uvSphere(0.1f, 10, 10);
    proton.position = glm::vec3(0.0f, 0.0f, 0.0f);
    electron.position = glm::vec3(0.0f, 0.0f, 0.3f);
    glm::vec3 protonVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 electronVelocity = glm::vec3(0.5f, 0.5f, 0.0f);
    glm::vec3 protonAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 electronAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 distanceVec;
    glm::vec3 dirVec;
    float distanceSquared;
    float countDown = 10.0f;

    lastFrame = glfwGetTime();
    meshShader.use();
    mesh.bufferToGPU();
    electron.bufferToGPU();
    proton.bufferToGPU();
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        // Inputs and updates
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        countDown -= deltaTime;

        // Canvas reset
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (countDown <= 0.0f)
        {
            for (int i = 0; i < 100; i++)
            {
                float dT = deltaTime * float(i) / 100.0f;
                distanceVec = (electron.position - proton.position);
                distanceSquared = glm::dot(distanceVec, distanceVec);
                dirVec = normalize(distanceVec);
                electronAcceleration = accel_on_e_from_p_per_m2 * distanceSquared * dirVec;
                protonAcceleration = accel_on_p_from_e_per_m2 * distanceSquared * dirVec;
                electronVelocity += electronAcceleration * dT;
                protonVelocity += protonAcceleration * dT;
                electron.position += electronVelocity * dT;
                proton.position += protonVelocity * dT;
            }
        }

        spotLight.position += (camera.position - spotLight.position) * deltaTime * 5.0f;
        spotLight.direction += (camera.front - spotLight.direction) * deltaTime * 5.0f;
        camera.updateProjection();
        camera.updateView();
        //mesh.calculateModel();
        //mesh.calculateNormal();
        electron.calculateModel();
        electron.calculateNormal();
        proton.calculateModel();
        proton.calculateNormal();
        meshShader.use();

        setSpotLight(spotLightUniform, spotLight);
        setPointLight(pointLightUniform, pointLight);
        setDirLight(dirLightUniform, dirLight);
        setMaterial(materialUniform, material);
        setMesh(meshUniform, mesh);
        setCamera(cameraUniform, camera);
        setInt(numSpotLightsUniform, 1);
        setInt(numPointLightsUniform, 1);
        setInt(numDirLightsUniform, 1);
        setMesh(meshUniform, electron);
        electron.draw();
        setMesh(meshUniform, proton);
        proton.draw();
        //mesh.draw();

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


void GLAPIENTRY debug_message_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam)
{
    std::string _source;
    std::string _type;
    std::string _severity;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UNDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    case GL_DEBUG_TYPE_OTHER:
    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION"; //return;
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    std::cout << "OPENGL DEBUG MESSAGE:" << std::endl;
    std::cout << "MESSAGE ID: " << id << std::endl;
    std::cout << "MESSAGE TYPE: " << _type << std::endl;
    std::cout << "MESSAGE SEVERITY: " << _severity << std::endl;
    std::cout << "MESSAGE SOURCE: " << _source << std::endl;
    std::cout << "MESSAGE:" << std::endl << message << std::endl << std::endl;
}
