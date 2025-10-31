#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal;

uniform float frequency;
uniform float amplitude;
uniform int layers;
uniform float layerRatio;
uniform float layerOffset;
uniform float t;

void main()
{
    float yOffset = 0.0;
    float ratio = 1.0;

    FragPos = aPos;
    Normal = vec3(0.0, 1.0, 0.0);

    gl_Position = projection * view * model * vec4(FragPos, 1.0);
}
