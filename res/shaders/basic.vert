#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform bool u_IsInstanced;
uniform bool isGrassWind;
uniform bool isBambooWind;
uniform float u_Time;

void main()
{

	mat4 finalModel = u_IsInstanced ? (model * aInstanceMatrix) : model;

	vec3 pos = aPos;
	if (isGrassWind || isBambooWind)
	{
		float windSpeed;
		if(isBambooWind)
			windSpeed = 1.0;
		else
			windSpeed = 2.5;
		float windStrength = 0.15;
		
		float mask = pos.x; 


		float wave = sin(u_Time * windSpeed + finalModel[3].x + finalModel[3].z) * windStrength;

		pos.x += wave * mask;
		pos.z += wave * 0.5 * mask;
	}

	gl_Position = projection * view * finalModel * vec4(pos, 1.0);
	TexCoord = aTexCoord;
	FragPos = vec3(finalModel * vec4(pos, 1.0f));
	//FragPos = pos;
	Normal = mat3(transpose(inverse(finalModel))) * aNormal;
	//Normal = aNormal;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}