#version 460 core
struct Material {
    vec3 baseDiffuseColor;
    vec3 baseSpecularColor;

    // Diffuse Textures
    sampler2D diffuseTextures[8];
    int numDiffuseTextures;

    // Specular Textures
    sampler2D specularTextures[8];
    int numSpecularTextures;

    float shininess;
};

struct DirLight {
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

struct Camera {
    mat4 projection; // Camera projection transformation matrix
    mat4 view; // Camera view transformation matrix
    vec3 position;
};

uniform Material material;
uniform PointLight pointLights[8];
uniform SpotLight spotLights[8];
uniform DirLight dirLights[8];

uniform int numPointLights;
uniform int numSpotLights;
uniform int numDirLights;

uniform Camera camera;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

//uniform vec3 viewPos;

vec4 calculateDiffuseColor();
vec4 calculateSpecularColor();
vec3 lightFromPointLight(PointLight, FragmentMaterial);
vec3 lightFromDirLight(DirLight, FragmentMaterial);
vec3 lightFromSpotLight(SpotLight, FragmentMaterial);
vec3 lightFromPointLights(FragmentMaterial);
vec3 lightFromDirLights(FragmentMaterial);
vec3 lightFromSpotLights(FragmentMaterial);

void main()
{

    FragmentMaterial mat;
    mat.diffuse = vec3(calculateDiffuseColor());
    mat.specular = vec3(calculateSpecularColor());
    mat.ambient = mat.diffuse;
    mat.shininess = material.shininess;

    vec3 color = lightFromPointLights(mat);
    color += lightFromDirLights(mat);
    color += lightFromSpotLights(mat);

    //FragColor = vec4(color, 1.0);
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

// Blends the diffuse base color with each diffuse texture
vec4 calculateDiffuseColor()
{
    vec4 diffuse = vec4(material.baseDiffuseColor, 1.0);
    int size = min(material.numDiffuseTextures, 8);
    for (int i = 0; i < size; i++)
    {
        diffuse += texture(material.diffuseTextures[i], TexCoords);
    }
    return diffuse;
}

// Blends the specular base color with each specular texture
vec4 calculateSpecularColor()
{
    vec4 specular = vec4(material.baseSpecularColor, 1.0);
    int size = min(material.numSpecularTextures, 8);
    for (int i = 0; i < size; i++)
    {
        specular += texture(material.specularTextures[i], TexCoords);
    }
    return specular;
}

vec3 lightFromPointLights(FragmentMaterial mat)
{
    vec3 result = vec3(0.0);
    int numLights = min(numPointLights, 8);
    for (int i = 0; i < numLights; i++)
    {
        result = result + lightFromPointLight(pointLights[i], mat);
    }
    return result;
}

vec3 lightFromDirLights(FragmentMaterial mat)
{
    vec3 result = vec3(0.0);
    int numLights = min(numDirLights, 8);
    for (int i = 0; i < numLights; i++)
    {
        result = result + lightFromDirLight(dirLights[i], mat);
    }
    return result;
}

vec3 lightFromSpotLights(FragmentMaterial mat)
{
    vec3 result = vec3(0.0);
    int numLights = min(numSpotLights, 8);
    for (int i = 0; i < numLights; i++)
    {
        result = result + lightFromSpotLight(spotLights[i], mat);
    }
    return result;
}

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
    vec3 viewDir = normalize(camera.position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * mat.specular;

    vec3 result = ambient + diffuse + specular;

    // Divide brightness by the length squared
    float dist = distance(light.position, FragPos);
    result = light.intensity * result / (light.quadratic * dist * dist + light.linear * dist + light.constant);

    return result;
}


vec3 lightFromDirLight(DirLight light, FragmentMaterial mat)
{
    // ambient
    vec3 ambient = light.ambient * mat.ambient;

    // diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * mat.diffuse;

    // specular
    vec3 viewDir = normalize(camera.position - FragPos);
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
    vec3 viewDir = normalize(camera.position - FragPos);
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
