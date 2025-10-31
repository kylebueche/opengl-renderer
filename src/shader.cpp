#include "../include/shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader()
{
    ID = 0;
    shaders = std::vector<unsigned int>();
}

Shader::~Shader()
{
    // Silently ignored if ID = 0
    glDeleteProgram(ID);
}

void Shader::compileVertexShader(const char* shaderPath)
{
    compileShader(shaderPath, GL_VERTEX_SHADER);
}

void Shader::compileFragmentShader(const char* shaderPath)
{
    compileShader(shaderPath, GL_FRAGMENT_SHADER);
}

void Shader::compileComputeShader(const char* shaderPath)
{
    compileShader(shaderPath, GL_COMPUTE_SHADER);
}

void Shader::compileTessControlShader(const char* shaderPath)
{
    compileShader(shaderPath, GL_TESS_CONTROL_SHADER);
}

void Shader::compileTessEvalShader(const char* shaderPath)
{
    compileShader(shaderPath, GL_TESS_EVALUATION_SHADER);
}

void Shader::compileGeometryShader(const char* shaderPath)
{
    compileShader(shaderPath, GL_GEOMETRY_SHADER);
}

void Shader::compileShader(const char* shaderPath, GLenum shaderType)
{
    std::ifstream shaderFile;
    std::stringstream shaderStream;
    std::string code;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        shaderFile.open(shaderPath);
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        code = shaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER_FILEREAD_UNSUCCESSFUL\n" << 
            "Trying to access " << shaderPath << std::endl <<
            e.what() << std::endl;
    }
    const char* shaderCode = code.c_str();
    int success;
    char infoLog[512];

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_FAILED\n" << shaderPath << std::endl << infoLog << std::endl;
    }
    
    shaders.push_back(shader);
}

void Shader::linkShaders()
{
    int success;
    char infoLog[512];

    ID = glCreateProgram();
    for (int i = 0; i < shaders.size(); i++)
    {
        glAttachShader(ID, shaders[i]);
    }
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_LINKING_FAILED\n" << infoLog << std::endl;
    }

    for (int i = 0; i < shaders.size(); i++)
    {
        glDeleteShader(shaders[i]);
    }
}

void Shader::use()
{
    glUseProgram(ID);
}

GLint Shader::getUniform(const std::string &name) const
{
    return glGetUniformLocation(ID, name.c_str());
}

void setBool(GLint uniform, bool value)
{
    glUniform1i(uniform, (int) value);
}

void setInt(GLint uniform, int value)
{
    glUniform1i(uniform, value);
}

void setFloat(GLint uniform, float value)
{
    glUniform1f(uniform, value);
}

void setVec2(GLint uniform, glm::vec2 vec)
{
    glUniform2f(uniform, vec.x, vec.y);
}

void setVec3(GLint uniform, glm::vec3 vec)
{
    glUniform3f(uniform, vec.x, vec.y, vec.z);
}

void setVec4(GLint uniform, glm::vec4 vec)
{
    glUniform4f(uniform, vec.x, vec.y, vec.z, vec.w);
}

void setMat4(GLint uniform, glm::mat4 mat)
{
    glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(mat));
}
