#version 460 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
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

    float quadratic;
    float linear;
    float constant;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float angle;
    float fadeAngle;
    float intensity;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float quadratic;
    float linear;
    float constant;
};

struct FragmentMaterial {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
};

uniform Material material;
uniform PointLight pointLight;
uniform SpotLight spotLight;
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
    float dist = distance(light.position, FragPos);
    result = light.intensity * result / (light.quadratic * dist * dist + light.linear * dist + light.constant);
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

vec3 lightFromSpotLight(SpotLight light, FragmentMaterial mat)
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
    float cos_theta = dot(lightDir, normalize(-light.direction));
    float epsilon = cos(radians(light.angle)) - cos(radians(light.fadeAngle));
    float spotIntensity = clamp((cos_theta - cos(radians(light.fadeAngle))) / epsilon, 0.0, 1.0);

    vec3 result = ambient + diffuse + specular;
    float dist = distance(light.position, FragPos);
    // Divide brightness by the length squared
    result = spotIntensity * light.intensity * result / (light.quadratic * dist * dist + light.linear * dist + light.constant);

    return result;
}

void main()
{
    FragmentMaterial mat;
    mat.diffuse = vec3(texture(material.diffuse, TexCoords));
    mat.specular = vec3(texture(material.specular, TexCoords));
    mat.ambient = mat.diffuse;
    mat.shininess = material.shininess;

    vec3 color = lightFromPointLight(pointLight, mat);
    color += lightFromDirectionalLight(dirLight, mat);
    color += lightFromSpotLight(spotLight, mat);

    FragColor = vec4(color, 1.0);
}
