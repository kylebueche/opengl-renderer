#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <numbers>

class Mesh
{
    public:
        float *vertices;
        float *faces;

        Mesh();
        Mesh(int numPoints);
        Mesh(int numPoints, int numFaces);
        ~Mesh();
};

Mesh fibonnacciSphere(int samples);
Mesh triangl();

// OpenGL functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Mesh functions

// Camera Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSourceOrange = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

const char *fragmentShaderSourceYellow = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\0";
    
int main()
{
	unsigned int vertexShader = 0;
	unsigned int fragmentShaderYellow = 0;
	unsigned int fragmentShaderOrange = 0;
	unsigned int shaderProgram1 = 0;
	unsigned int shaderProgram2 = 0;
    unsigned int VAOs[2] = { 0 };
    unsigned int VBOs[2] = { 0 };
    unsigned int EBOs[2] = { 0 };
	int success = 0;
	char infoLog[512];
    float vertices1[] =
    {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices1[] = 
    {
        0, 1, 3,
        1, 2, 3
    };
    float vertices2[] =
    {
        1.0f, 1.0f, 0.0f,
        1.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        0.5f, 1.0f, 0.0f
    };
    unsigned int indices2[] = 
    {
        0, 1, 3,
        1, 2, 3
    };
    // GLFW Setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Resize function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD Setup
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderOrange, 1, &fragmentShaderSourceOrange, NULL);
	glCompileShader(fragmentShaderOrange);
	glGetShaderiv(fragmentShaderOrange, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderOrange, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderYellow, 1, &fragmentShaderSourceYellow, NULL);
	glCompileShader(fragmentShaderYellow);
	glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderYellow, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	shaderProgram1 = glCreateProgram();
	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShaderOrange);
	glLinkProgram(shaderProgram1);

    glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

	shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShaderYellow);
	glLinkProgram(shaderProgram2);

    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderYellow);
    glDeleteShader(fragmentShaderOrange);

    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);
    
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram1);
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgram2);
        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(2, EBOs);
    glDeleteProgram(shaderProgram1);
    glDeleteProgram(shaderProgram2);

    glfwTerminate();
    return 0;
}

// OpenGL functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// Mesh functions

Mesh::Mesh()
{
    vertices = NULL;
    faces = NULL;
}

Mesh::Mesh(int numPoints)
{
    vertices = new float[3 * numPoints];
    faces = NULL;
}

Mesh::Mesh(int numPoints, int numFaces)
{
    vertices = new float[3 * numPoints];
    faces = new float[3 * numFaces];
}

Mesh::~Mesh()
{
    if (vertices != NULL)
    {
        delete(vertices);
    }
    if (faces != NULL)
    {
        delete(faces);
    }
}

Mesh fibonnacciSphere(int samples)
{
    Mesh mesh = Mesh(samples);
    float goldenAngle = std::numbers::pi * (sqrt(5.0f) - 1.0f);
    float x, y, z = 0;
    float radius = 0;
    float theta = 0;

    for (int i = 0; i < samples; i++)
    {
        y = 1 - (i / float(samples - 1)) * 2;
        radius = sqrt(1 - y * y);
        
        theta = goldenAngle * i;

        x = cos(theta) * radius;
        z = sin(theta) * radius;

        mesh.vertices[i] = x;
        mesh.vertices[i + 1] = y;
        mesh.vertices[i + 2] = z;
    }
    return mesh;
}

Mesh triangl()
{
    Mesh mesh = Mesh(3, 1);
    mesh.vertices[0] = -0.5f;
    mesh.vertices[1] = -0.5f;
    mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f;
    mesh.vertices[4] = -0.5f;
    mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = 0.0f;
    mesh.vertices[7] = 1.0f;
    mesh.vertices[8] = 0.0f;
    mesh.faces[0] = 0;
    mesh.faces[1] = 3;
    mesh.faces[2] = 6;
    return mesh;
}
