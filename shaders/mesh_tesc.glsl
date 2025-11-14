#version 460 core

layout (vertices=3) out;

in vec3 FragPos[];
out vec3 FragPosition[];

in vec3 Normal[];
out vec3 Normals[];

in vec2 TexCoords[];
out vec2 TextureCoords[];

void main() {
    gl_out[gl_InvocationID].gl_position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
}