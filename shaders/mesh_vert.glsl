#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// Instanced
layout (location = 3) in mat4 aModel;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal;

// if false, use model uniform
// if true, use aModels layout
uniform bool renderInstanced;

void main()
{
    mat4 resultingModel = renderInstanced ? aModel : model;

    mat4 mat = mat4(1.0);

    FragPos = vec3(resultingModel * vec4(aPos, 1.0));
    Normal = aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
