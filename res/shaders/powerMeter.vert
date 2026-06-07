#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_width;
uniform float u_height;

void main()
{
    TexCoords = aTexCoords;
//    vec3 center = vec3(model * vec4(0.0, 0.0, 0.0, 1.0));
//    vec3 worldPos = center + (cameraRight * aPos.x * u_width) + (cameraUp * aPos.z * u_height);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}