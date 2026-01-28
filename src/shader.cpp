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
    if (ID != 0)
    {
        glDeleteProgram(ID);
    }
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
    catch(std::ifstream::failure& e)
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
    GLint i = glGetUniformLocation(ID, name.c_str());
    if (i == -1)
    {
        std::cout << "ERROR::SHADER_UNIFORM_DOESNT_EXIST: \"" << name.c_str() << "\"" << std::endl;
    }
    return i;
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

void setMat3(GLint uniform, glm::mat3 mat)
{
    glUniformMatrix3fv(uniform, 1, GL_FALSE, glm::value_ptr(mat));
}

DirLightUniform Shader::getDirLightUniform(std::string uniformName)
{
    DirLightUniform uniform;
    uniform.direction = getUniform((uniformName + ".direction").c_str());
    uniform.intensity = getUniform((uniformName + ".intensity").c_str());

    uniform.ambient = getUniform((uniformName + ".ambient").c_str());
    uniform.diffuse = getUniform((uniformName + ".diffuse").c_str());
    uniform.specular = getUniform((uniformName + ".specular").c_str());
    return uniform;
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
    return light;
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
    return light;
}

MaterialUniform Shader::getMaterialUniform(std::string uniformName)
{
    MaterialUniform uniform;
    uniform.baseDiffuseColor = getUniform((uniformName + ".baseDiffuseColor").c_str());
    uniform.baseSpecularColor = getUniform((uniformName + ".baseSpecularColor").c_str());
    for (int i = 0; i < 8; i++)
    {
        uniform.diffuseTextures[i] = getUniform((uniformName + ".diffuseTextures[" + std::to_string(i) + "]").c_str());
        uniform.specularTextures[i] = getUniform((uniformName + ".specularTextures[" + std::to_string(i) + "]").c_str());
    }
    uniform.numDiffuseTextures = getUniform((uniformName + ".numDiffuseTextures").c_str());
    uniform.numSpecularTextures = getUniform((uniformName + ".numSpecularTextures").c_str());
    uniform.shininess = getUniform((uniformName + ".shininess").c_str());
    return uniform;
}

CameraUniform Shader::getCameraUniform(std::string uniformName)
{
    CameraUniform uniform;
    uniform.position = getUniform((uniformName + ".position").c_str());
    uniform.projection = getUniform((uniformName + ".projection").c_str());
    uniform.view = getUniform((uniformName + ".view").c_str());
    return uniform;
}

MeshUniform Shader::getMeshUniform(std::string uniformName)
{
    MeshUniform uniform;
    uniform.model = getUniform((uniformName + ".model").c_str());
    uniform.normal = getUniform((uniformName + ".normal").c_str());
    return uniform;
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

void setMaterial(MaterialUniform uniform, Material material)
{
    setVec3(uniform.baseDiffuseColor, material.baseDiffuseColor);
    setVec3(uniform.baseSpecularColor, material.baseSpecularColor);
    int nDiffuse = std::min(8, int(material.diffuseTextures.size()));
    int nSpecular = std::min(8, int(material.specularTextures.size()));
    for (int i = 0; i < nDiffuse; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, material.diffuseTextures[i].id);
        setInt(uniform.diffuseTextures[i], GL_TEXTURE0 + i);
    }
    for (int i = 0; i < nSpecular; i++)
    {
        glActiveTexture(GL_TEXTURE0 + nDiffuse + i);
        glBindTexture(GL_TEXTURE_2D, material.specularTextures[i].id);
        setInt(uniform.specularTextures[i], GL_TEXTURE0 + nDiffuse + i);
    }
    glActiveTexture(GL_TEXTURE0);
    setInt(uniform.numDiffuseTextures, nDiffuse);
    setInt(uniform.numSpecularTextures, nSpecular);
    setFloat(uniform.shininess, material.shininess);
}

void setCamera(CameraUniform uniform, Camera camera)
{
    setVec3(uniform.position, camera.position);
    setMat4(uniform.projection, camera.projection);
    setMat4(uniform.view, camera.view);
}

void setMesh(MeshUniform uniform, Mesh& mesh)
{
    setMat4(uniform.model, mesh.model);
    setMat3(uniform.normal, mesh.normal);
}