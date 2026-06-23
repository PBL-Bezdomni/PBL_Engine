#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoord;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

uniform bool u_IsInstanced;

void main()
{

    mat4 finalModel = u_IsInstanced ? (model * aInstanceMatrix) : model;
    TexCoord = aTexCoord;
    gl_Position = lightSpaceMatrix * finalModel * vec4(aPos, 1.0);
}