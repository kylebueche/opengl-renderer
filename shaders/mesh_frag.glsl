#version 460 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    float intensity;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float intensity;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct FragmentMaterial {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
};

uniform Material material;
uniform PointLight light;
uniform DirectionalLight dirLight;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

vec3 lightFromPointLight(PointLight light, FragmentMaterial mat)
{
    // ambient
    vec3 ambient = light.ambient * mat.ambient;

    // diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * mat.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * mat.specular;

    vec3 result = ambient + diffuse + specular;

    // Divide brightness by the length squared
    result = light.intensity * result / dot(light.position - FragPos, light.position - FragPos);
    return result;
}

vec3 lightFromDirectionalLight(DirectionalLight light, FragmentMaterial mat)
{
    // ambient
    vec3 ambient = light.ambient * mat.ambient;

    // diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * mat.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * mat.specular;

    vec3 result = ambient + diffuse + specular;

    // Divide brightness by the length squared
    result = light.intensity * result;
    return result;
}

void main()
{
    FragmentMaterial mat;
    mat.diffuse = vec3(texture(material.diffuse, TexCoords));
    mat.specular = vec3(texture(material.specular, TexCoords));
    mat.ambient = mat.diffuse;
    mat.shininess = material.shininess;

    vec3 color = lightFromPointLight(light, mat);
    color += lightFromDirectionalLight(dirLight, mat);

    FragColor = vec4(color, 1.0);
}
