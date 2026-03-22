#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform float refraction_index;
uniform vec3 rgb_refraction_index;
uniform bool use_chromatic_refraction;

void main()
{
    vec3 I = normalize(Position - cameraPos);

    if (use_chromatic_refraction)
    {
        float red_ratio = 1.00 / rgb_refraction_index.x;
        float green_ratio = 1.00 / rgb_refraction_index.y;
        float blue_ratio = 1.00 / rgb_refraction_index.z;
        vec3 red = refract(I, normalize(Normal), red_ratio);
        vec3 green = refract(I, normalize(Normal), green_ratio);
        vec3 blue = refract(I, normalize(Normal), blue_ratio);
        FragColor = vec4(texture(skybox, red).r, texture(skybox, green).g, texture(skybox, blue).b, 1.0);
    }
    else
    {
        float ratio = 1.00 / refraction_index;
        vec3 R = refract(I, normalize(Normal), ratio);
        FragColor = vec4(texture(skybox, R).r, texture(skybox, R).g, texture(skybox, R).b, 1.0);
    }
}