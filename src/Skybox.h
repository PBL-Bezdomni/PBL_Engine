#pragma once

#include <vector>
#include "Shader.h"
#include "Texture.h"

using namespace std;
class Skybox
{
private:
	unsigned int m_SkyboxVAO;
	unsigned int m_SkyboxVBO;
	Texture m_SkyboxTex;
	Shader m_Shader = Shader();
	
	void GenerateSkybox();
	void LoadCubemapTextures(vector<string> faces, Texture* texture);
public:
	Skybox() = default;
	Skybox(Shader& shader);
	void DrawSkybox(glm::mat4 view, glm::mat4 projection);
};
