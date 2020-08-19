#version 330 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {    
    sampler2D diffuse;
    sampler2D specular;

    vec3 color;
    float shininess;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
//uniform Light light;

//uniform

#define NR_POINT_LIGHTS 30;
uniform PointLight pointLight[30];
uniform SpotLight spotLight[30];
uniform DirLight dirLight[30];

uniform int numberOfPointLights;
uniform int numberOfSpotLights;
uniform int numberOfDirectionalLights;

out vec4 FragColor;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);


//const float offset = 1.0 / 128.0;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result;// = vec3(0.0);

    for(int i = 0; i < numberOfPointLights; i++)
    {
        result += CalcPointLight(pointLight[i], norm, FragPos, viewDir);
    }
    for(int i = 0; i < numberOfSpotLights; i++)
    {
        result += CalcSpotLight(spotLight[i], norm, FragPos, viewDir);
    }
    for(int i = 0; i < numberOfDirectionalLights; i++)
    {
        result += CalcDirLight(dirLight[i], norm, viewDir);
    }


//    result += CalcDirLight(dirLight, norm, viewDir);
//    result += CalcPointLight(pointLight, norm, FragPos, viewDir);
//    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    //TODO implement for more light sources
    //    for(int i = 0; i < 4; i++)
    //    {
    //	result += CalcPointLight(pointlights[i], norm, FragPos, viewDir);
    //    }

    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * material.color * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * material.color * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * material.color * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * material.color * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 10);

    // combine results
    vec3 ambient  = light.ambient  * material.color * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * material.color * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    diffuse *= intensity;
    specular *= intensity;
    return (ambient + diffuse + specular);

}
