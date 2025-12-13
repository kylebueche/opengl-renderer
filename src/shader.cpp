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

DirLightUniform Shader::getDirLightUniform(std::string uniformName)
{
    DirLightUniform uniform;
    uniform.direction = getUniform((uniformName + ".direction").c_str());
    uniform.intensity = getUniform((uniformName + ".intensity").c_str());

    uniform.ambient = getUniform((uniformName + ".ambient").c_str());
    uniform.diffuse = getUniform((uniformName + ".diffuse").c_str());
    uniform.specular = getUniform((uniformName + ".specular").c_str());
}
PointLightUniform Shader::getPointLightUniform(std::string uniformName)
{
    PointLightUniform light;
    light.position = getUniform((uniformName + ".position").c_str());
    light.intensity = getUniform((uniformName + ".intensity").c_str());

    light.ambient = getUniform((uniformName + ".ambient").c_str());
    light.diffuse = getUniform((uniformName + ".diffuse").c_str());
    light.specular = getUniform((uniformName + ".specular").c_str());

    light.constant = getUniform((uniformName + ".constant").c_str());
    light.linear = getUniform((uniformName + ".linear").c_str());
    light.quadratic = getUniform((uniformName + ".quadratic").c_str());
}
SpotLightUniform Shader::getSpotLightUniform(std::string uniformName)
{
    SpotLightUniform light;
    light.position = getUniform((uniformName + ".position").c_str());
    light.direction = getUniform((uniformName + ".direction").c_str());
    light.angle = getUniform((uniformName + ".angle").c_str());
    light.fadeAngle = getUniform((uniformName + ".fadeAngle").c_str());
    light.intensity = getUniform((uniformName + ".intensity").c_str());

    light.ambient = getUniform((uniformName + ".ambient").c_str());
    light.diffuse = getUniform((uniformName + ".diffuse").c_str());
    light.specular = getUniform((uniformName + ".specular").c_str());

    light.constant = getUniform((uniformName + ".constant").c_str());
    light.linear = getUniform((uniformName + ".linear").c_str());
    light.quadratic = getUniform((uniformName + ".quadratic").c_str());
}

void setDirLight(DirLightUniform uniform, DirLight dirLight)
{
    setVec3(uniform.direction, dirLight.direction);
    setFloat(uniform.intensity, dirLight.intensity);

    setVec3(uniform.ambient, dirLight.ambient);
    setVec3(uniform.diffuse, dirLight.diffuse);
    setVec3(uniform.specular, dirLight.specular);
}

void setPointLight(PointLightUniform uniform, PointLight pointLight)
{
    setVec3(uniform.position, pointLight.position);
    setFloat(uniform.intensity, pointLight.intensity);

    setVec3(uniform.ambient, pointLight.ambient);
    setVec3(uniform.diffuse, pointLight.diffuse);
    setVec3(uniform.specular, pointLight.specular);

    setFloat(uniform.constant, pointLight.constant);
    setFloat(uniform.linear, pointLight.linear);
    setFloat(uniform.quadratic, pointLight.quadratic);
}


void setSpotLight(SpotLightUniform uniform, SpotLight spotLight)
{
    setVec3(uniform.position, spotLight.position);
    setVec3(uniform.direction, spotLight.direction);
    setFloat(uniform.angle, spotLight.angle);
    setFloat(uniform.fadeAngle, spotLight.fadeAngle);
    setFloat(uniform.intensity, spotLight.intensity);

    setVec3(uniform.ambient, spotLight.ambient);
    setVec3(uniform.diffuse, spotLight.diffuse);
    setVec3(uniform.specular, spotLight.specular);

    setFloat(uniform.constant, spotLight.constant);
    setFloat(uniform.linear, spotLight.linear);
    setFloat(uniform.quadratic, spotLight.quadratic);
}