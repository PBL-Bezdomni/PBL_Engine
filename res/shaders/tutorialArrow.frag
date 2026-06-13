#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 u_ArrowColor;

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if (texColor.a < 0.1) discard;
    vec3 finalColor = texColor.rgb * u_ArrowColor;
    FragColor = vec4(finalColor, texColor.a);
}