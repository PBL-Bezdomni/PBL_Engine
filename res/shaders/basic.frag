#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 NormalColor;
layout (location = 2) out vec3 GlowColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform bool u_IsHighlighted;
uniform vec3 u_GlowColor;

uniform sampler2D waterTexture;
uniform bool isWater;
uniform float u_Time;

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
uniform sampler2D texture_normal1;

uniform sampler2D shadowMap;
uniform sampler2D staticShadowMap;

uniform vec3 viewPos;
uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;

uniform bool useDirLight;
uniform bool usePointLight;
uniform bool useSpotLight;

uniform bool useNormal;

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
uniform Light spotLight;

vec3 CalcDirLight(vec3 norm, vec3 viewDir, vec3 objectColor);
vec3 CalcPointLight(vec3 norm, vec3 viewDir, vec3 objectColor);
vec3 CalcSpotLight(Light light, vec3 norm, vec3 viewDir, vec3 objectColor);

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D map);



void main()
{
    //FragColor = texture(texture_diffuse1, TexCoord);

    vec3 norm;
    if (useNormal)
    {
        vec3 normal = texture(texture_normal1, TexCoord).rgb;
        norm = normalize(normal * 2.0 - 1.0);
    }
    else
    {
        norm = normalize(Normal);
    }
    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 texColor;
    if (isWater) 
    {
        vec2 flowSpeed = vec2(-0.01, 0.0); 
        vec2 animatedUV = TexCoord + (flowSpeed * u_Time);
        texColor = texture(waterTexture, animatedUV); 
    } 
    else 
    { 
        texColor = texture(texture_diffuse1, TexCoord);
    }
    texColor.rgb = pow(texColor.rgb, vec3(2.2));
    if (texColor.a < 0.1)
    {
        discard;
    }
    vec3 result = vec3(0.0);
    if (useDirLight)
    {
        result += CalcDirLight(norm, viewDir, vec3(texColor));
    }
    if (usePointLight)
    {
        result += CalcPointLight(norm, viewDir, vec3(texColor));
    }
    if (useSpotLight)
    {
        result += CalcSpotLight(spotLight, norm, viewDir, vec3(texColor));
    }

    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, texColor.a);
    NormalColor = normalize(Normal);

    if (u_IsHighlighted) {
        GlowColor = u_GlowColor * 1.5;
    } else {
        GlowColor = vec3(0.0);
    }

}

vec3 CalcDirLight(vec3 norm, vec3 viewDir, vec3 objectColor)
{
    float sunIntensity = 2.5f;
    vec3 lightDir = normalize(-dirLight.direction);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 actualLightColor = dirLight.color * sunIntensity;

    vec3 ambient = ambientStrength * actualLightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * actualLightColor;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * actualLightColor;

    float shadow = ShadowCalculation(FragPosLightSpace, shadowMap);                      
    float staticShadow = ShadowCalculation(FragPosLightSpace, staticShadowMap);
    // Comments left for demonstration purpose
    // Both static and dynamic shadow maps

    float combinedShadow = max(shadow, staticShadow);
    vec3 shadowColorTint = vec3(0.15, 0.20, 0.35) * diffuse;
    vec3 finalLight = ambient + mix(diffuse + specular, shadowColorTint, combinedShadow);
    float rimFactor = 1.0 - max(dot(viewDir, norm), 0.0);
    rimFactor = smoothstep(0.6, 1.0, rimFactor); 
    vec3 rimLight = rimFactor * vec3(0.3, 0.5, 0.6) * diff;
    
    return (finalLight + rimLight) * objectColor;


    //return (ambient + (1.0 - combinedShadow) * (diffuse + specular)) * objectColor;
    // Only static shadow map
//    return  (ambient + (1.0 - staticShadow) * (diffuse + specular)) * objectColor;
    // Only dynamic shadow map
//    return  (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor; 


    //return (ambient + diffuse + specular) * vec3(objectColor);
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

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D map)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // Защита от теней за пределами источника света
    if(projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction); 
    
    // Немного смягчили bias для мягких теней
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(map, 0);
    
    // Настройки мягкости
    int radius = 2;          // Радиус размытия (2 = 5x5, 3 = 7x7)
    float spread = 2.0;      // Насколько сильно пиксели отступают друг от друга
    
    float samples = pow((radius * 2.0 + 1.0), 2.0); // Общее количество сэмплов

    for(int x = -radius; x <= radius; ++x)
    {
        for(int y = -radius; y <= radius; ++y)
        {
            float pcfDepth = texture(map, projCoords.xy + vec2(x, y) * texelSize * spread).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= samples;
        
    return shadow;
}