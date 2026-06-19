#include "Skybox.h"
#include "Engine//Loader.h"

#include <glad/glad.h>
#include <stb_image.h>
//TODO remove beneath include
#include <GLFW/glfw3.h>

#include "EngineConsts.h"
#include "Model.h"

Skybox::Skybox(Shader& shader)
{
	m_Shader = shader;
	GenerateSkybox();
	m_Shader.Use();
	m_Shader.SetInt("skybox", 0);
}

void Skybox::GenerateSkybox()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	vector<string> cubemapFaces { 
		"res/textures/cubemap/right.jpg", 
		"res/textures/cubemap/left.jpg",
		"res/textures/cubemap/top.jpg", 
		"res/textures/cubemap/bottom.jpg", 
		"res/textures/cubemap/front.jpg", 
		"res/textures/cubemap/back.jpg"
	};

	m_Shader.Use();
	glGenVertexArrays(1, &m_SkyboxVAO);
	glGenBuffers(1, &m_SkyboxVBO);
	glBindVertexArray(m_SkyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_SkyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	LoadCubemapTextures(cubemapFaces, &m_SkyboxTex);
}

void Skybox::LoadCubemapTextures(vector<string> faces, Texture* texture)
{
	texture->Path = "";
	texture->FileName = "";
	texture->Type = EngineConsts::DIFFUSE;
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load((Loader::RelativePath() + faces[i]).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			std::cout << "Cubemap texture loaded at path: " << Loader::RelativePath() + faces[i] << std::endl;
		}
		else
		{
			std::cout << "ERROR: Cubemap texture failed to load at path: {}" << Loader::RelativePath() + faces[i];
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	texture->ID = textureID;
}

void Skybox::DrawSkybox(glm::mat4 view, glm::mat4 projection)
{
	glDepthFunc(GL_LEQUAL);
	m_Shader.Use();
	m_Shader.SetMat4("view", view);
	m_Shader.SetMat4("projection", projection);
	glBindVertexArray(m_SkyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxTex.ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

