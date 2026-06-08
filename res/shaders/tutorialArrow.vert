#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform float u_width;
uniform float u_height;
uniform float rotationAngle;

void main()
{
    TexCoords = aTexCoords;

    vec2 local = vec2(aPos.x * u_width, aPos.z * u_height);
    mat2 rot = mat2(cos(rotationAngle), -sin(rotationAngle), sin(rotationAngle), cos(rotationAngle));

    local = rot * local;

    vec3 center = vec3(model * vec4(0.0, 0.0, 0.0, 1.0));

    vec3 worldPos = center + (cameraRight * local.x) + (cameraUp * local.y);

    gl_Position = projection * view * vec4(worldPos, 1.0);
}