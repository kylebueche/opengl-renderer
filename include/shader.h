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
};

void setBool(GLint uniform, bool value);
void setInt(GLint uniform, int value);
void setFloat(GLint uniform, float value);
void setVec2(GLint uniform, glm::vec2 vec);
void setVec3(GLint uniform, glm::vec3 vec);
void setVec4(GLint uniform, glm::vec4 vec);
void setMat4(GLint uniform, glm::mat4 mat);

#endif
