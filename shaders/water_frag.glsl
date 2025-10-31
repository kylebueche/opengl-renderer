#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 sunDir;

uniform float dt;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(sunDir);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular

    float frequency1 = 10.5;
    float frequency2 = 13.2;
    float frequency3 = 3.7;
    float frequency4 = 4.1;
    float frequency5 = 17.2;
    float frequency6 = 14.8;
    float speed = 0.5;
    float time = speed * dt;
    float height = sin(FragPos.x * frequency1 + time) * cos(FragPos.z * frequency4 - time);
    float height *= sin(FragPos.x * frequency2 - time) * cos(FragPos.z * frequency5 + time);
    float height *= sin(FragPos.x * frequency3 + time) * cos(FragPos.z * frequency6 - time);
    // normalize

    gl_FragDepth = gl_FragDepth + height;

    vec3 result = (ambient + diffuse) * objectColor;

    FragColor = vec4(result, 1.0);
}
