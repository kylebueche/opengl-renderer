#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 world;
uniform mat4 transform;

void main()
{
    gl_Position = world * transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
