#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 7) in ivec4 aBoneIDs;
layout (location = 8) in vec4 aWeights;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 bonesMatrices[MAX_BONES];

void main()
{
	mat4 boneTransform = mat4(0.0);
	bool hasBones = false;

	float totalWeight = 0.0;
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if (aBoneIDs[i] >= 0 && aBoneIDs[i] < MAX_BONES)
		{
			totalWeight += aWeights[i];
		}
	}

	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if (aBoneIDs[i] >= 0 && aBoneIDs[i] < MAX_BONES)
		{
			float normWeight = (totalWeight > 0.0) ? (aWeights[i] / totalWeight) : 0.0;
			boneTransform += bonesMatrices[aBoneIDs[i]] * normWeight;
			hasBones = true;
		}
	}

	if (!hasBones)
	{
		boneTransform = mat4(1.0);
	}

	vec4 localPosition = boneTransform * vec4(aPos, 1.0);
	vec3 localNormal = mat3(boneTransform) * aNormal;
    
	gl_Position = projection * view * model * localPosition;
	TexCoord = aTexCoord;
	FragPos = vec3(model * localPosition);
	Normal = mat3(transpose(inverse(model))) * localNormal;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}