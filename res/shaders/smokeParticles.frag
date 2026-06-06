#version 430

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 Color;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoord) * Color;
    if (texColor.a < 0.1)
    {
        discard;
    }

    FragColor = texColor;
}