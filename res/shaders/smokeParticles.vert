#version 430
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct Particle
{
    vec4 position;
    vec4 velocity;

    float life;
    float maxLife;
    float size;
    uint alive;

    vec4 color;
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
};

void main()
{
    Particle p = particles[gl_InstanceID];

    vec3 pos = p.position.xyz;
//    mat4 modelMatix = 

    gl_Position = projection * view * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}