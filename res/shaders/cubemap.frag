#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 NormalColor;
in vec3 TexCoord;
uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TexCoord);
	NormalColor = vec3(0.0);
}