#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// Instanced
//layout (location = 3) in mat4 aModel;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

struct Camera {
    mat4 projection; // Camera projection transformation matrix
    mat4 view; // Camera view transformation matrix
    vec3 position;
};

struct Mesh {
    mat4 model; // Mesh transformation matrix
    mat3 normal; // Mesh normal transformation matrix
};

uniform Mesh mesh;
uniform Camera camera;

//uniform mat4 projection;
//uniform mat4 view;
//uniform mat4 model;
//uniform mat3 normal;

// if false, use model uniform
// if true, use aModels layout
//uniform bool renderInstanced;

void main()
{
    //mat4 modelMatrix = renderInstanced ? aModel : model;
    //mat3 normalMatrix = transpose(inverse(mat3(model)));

    FragPos = vec3(mesh.model * vec4(aPos, 1.0));
    Normal = mesh.normal * aNormal;
    TexCoords = aTexCoords;

    // Converted to screenspace
    gl_Position = camera.projection * camera.view * vec4(FragPos, 1.0);
}
