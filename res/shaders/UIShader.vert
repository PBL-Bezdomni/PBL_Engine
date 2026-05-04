#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 pos = projection * model * vec4(aPos, 1.0);
	pos = vec4(pos.x, pos.y, 0, pos.w);
	gl_Position = pos;
	TexCoord = aTexCoord;
}