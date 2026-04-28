#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_diffuse5;
uniform sampler2D texture_diffuse6;
uniform sampler2D texture_diffuse7;
uniform sampler2D texture_diffuse8;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;
uniform sampler2D texture_specular5;
uniform sampler2D texture_specular6;
uniform sampler2D texture_specular7;
uniform sampler2D texture_specular8;

uniform vec3 viewPos;
uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;

uniform bool useDirLight;
uniform bool usePointLight;
uniform bool useSpotLight1;
uniform bool useSpotLight2;

struct Light {
    vec3 direction;
    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

uniform Light dirLight;
uniform Light pointLight;
uniform Light spotLight1;
uniform Light spotLight2;

vec3 CalcDirLight(vec3 norm, vec3 viewDir, vec3 objectColor);
vec3 CalcPointLight(vec3 norm, vec3 viewDir, vec3 objectColor);
vec3 CalcSpotLight(Light light, vec3 norm, vec3 viewDir, vec3 objectColor);

void main()
{
    //FragColor = texture(texture_diffuse1, TexCoord);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 texColor = texture(texture_diffuse1, TexCoord);
//    if (texColor.a < 0.1)
//    {
//        discard;
//    }
    vec3 result = vec3(0.0);
    if (useDirLight)
    {
        result += CalcDirLight(norm, viewDir, vec3(texColor));
    }
    if (usePointLight)
    {
        result += CalcPointLight(norm, viewDir, vec3(texColor));
    }
    if (useSpotLight1)
    {
        result += CalcSpotLight(spotLight1, norm, viewDir, vec3(texColor));
    }
    if (useSpotLight2)
    {
        result += CalcSpotLight(spotLight2, norm, viewDir, vec3(texColor));
    }

    FragColor = vec4(result, texColor.a);

}

vec3 CalcDirLight(vec3 norm, vec3 viewDir, vec3 objectColor)
{
    vec3 lightDir = normalize(-dirLight.direction);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = ambientStrength * dirLight.color;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * dirLight.color;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * dirLight.color;

    return (ambient + diffuse + specular) * vec3(objectColor);
}

vec3 CalcPointLight(vec3 norm, vec3 viewDir, vec3 objectColor)
{
    vec3 lightDir = normalize(pointLight.position - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));    

    vec3 ambient = ambientStrength * pointLight.color;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * pointLight.color;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    vec3 specular = specularStrength * spec * pointLight.color;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular) * objectColor;
}

vec3 CalcSpotLight(Light light, vec3 norm, vec3 viewDir, vec3 objectColor)
{
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = ambientStrength * light.color;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * light.color;

    diffuse *= intensity;
    specular *= intensity;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular) * objectColor;
}