#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float u_Progress;

void main()
{
    vec3 bgColor = vec3(0.2, 0.2, 0.2);
    vec3 fillColor = vec3(0.2, 0.9, 0.2);

    if (TexCoords.x <= u_Progress) 
    {
        FragColor = vec4(fillColor, 1.0);
    } 
    else 
    {
        FragColor = vec4(bgColor, 0.8);
    }
}