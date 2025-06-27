#version 460 core
out vec4 FragColor;
in vec2 TexCoord;

uniform float blend;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), blend);
}
