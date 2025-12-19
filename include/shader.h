#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <bits/stl_tempbuf.h>
#include "lights.h"
#include "material.h"
#include "camera.h"
#include "mesh.h"

struct DirLightUniform;
struct PointLightUniform;
struct SpotLightUniform;
struct MaterialUniform;
struct CameraUniform;
struct MeshUniform;

class Shader
{
public:
    // Program ID
    unsigned int ID;
    std::vector<unsigned int> shaders;

    Shader();
    ~Shader();

    void compileVertexShader(const char* shaderPath);
    void compileFragmentShader(const char* shaderPath);
    void compileComputeShader(const char* shaderPath);
    void compileTessControlShader(const char* shaderPath);
    void compileTessEvalShader(const char* shaderPath);
    void compileGeometryShader(const char* shaderPath);

    void compileShader(const char* shaderPath, GLenum shaderType);
    void linkShaders();

    void use();
    GLint getUniform(const std::string &name) const;

    DirLightUniform getDirLightUniform(std::string uniformName);
    PointLightUniform getPointLightUniform(std::string uniformName);
    SpotLightUniform getSpotLightUniform(std::string uniformName);
    MaterialUniform getMaterialUniform(std::string uniformName);
    CameraUniform getCameraUniform(std::string uniformName);
    MeshUniform getMeshUniform(std::string uniformName);
};

void setBool(GLint uniform, bool value);
void setInt(GLint uniform, int value);
void setFloat(GLint uniform, float value);
void setVec2(GLint uniform, glm::vec2 vec);
void setVec3(GLint uniform, glm::vec3 vec);
void setVec4(GLint uniform, glm::vec4 vec);
void setMat4(GLint uniform, glm::mat4 mat);
void setMat3(GLint uniform, glm::mat3 mat);
void setDirLight(DirLightUniform uniform, DirLight dirLight);
void setPointLight(PointLightUniform uniform, PointLight pointLight);
void setSpotLight(SpotLightUniform uniform, SpotLight spotLight);
void setMaterial(MaterialUniform uniform, Material material);
void setCamera(CameraUniform uniform, Camera camera);
void setMesh(MeshUniform uniform, Mesh& mesh);

struct DirLightUniform {
    GLint direction;
    GLint intensity;

    // Color
    GLint ambient;
    GLint diffuse;
    GLint specular;
};

struct PointLightUniform {
    GLint position;
    GLint intensity;

    // Color
    GLint ambient;
    GLint diffuse;
    GLint specular;

    // Attenuation
    GLint quadratic;
    GLint linear;
    GLint constant;
};

struct SpotLightUniform {
    GLint position;
    GLint direction;
    GLint angle;
    GLint fadeAngle;
    GLint intensity;

    // Color
    GLint ambient;
    GLint diffuse;
    GLint specular;

    // Attenuation
    GLint quadratic;
    GLint linear;
    GLint constant;
};

struct MaterialUniform
{
    GLint baseDiffuseColor;
    GLint baseSpecularColor;
    GLint diffuseTextures[8];
    GLint specularTextures[8];
    GLint numDiffuseTextures;
    GLint numSpecularTextures;
    GLint shininess;
};

struct CameraUniform
{
    GLint position;
    GLint view;
    GLint projection;
};

struct MeshUniform
{
    GLint model;
    GLint normal;
};

#endif
