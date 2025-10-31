#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 camera;
uniform mat4 transform;
uniform vec3 lightPos;

void main()
{
    gl_Position = camera * transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
