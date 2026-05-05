#include "UIManager.h"
#include "spdlog/spdlog.h"

void UIManager::Init(AssetManager* assetManager, WindowManager* windowManager)
{
	m_AssetManager = assetManager;
	m_WindowManager = windowManager;

	if (!m_AssetManager || !m_WindowManager)
	{
		spdlog::error("UIManager AssetManager or WindowManager is null");
		return;
	}

	UpdateProjection();
	InitRenderData();
}

void UIManager::SetUIStates()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void UIManager::ResetStates()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void UIManager::InitRenderData()
{
	float vertices[] = {
		0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UIManager::DrawSprite(Shader& shader, unsigned int textureID, glm::vec2 position, glm::vec2 size, float rotate)
{
	SetUIStates();

	shader.Use();
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	shader.SetMat4("model", model);
	shader.SetMat4("projection", m_Projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	shader.SetInt("texture_diffuse1", 0);

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	ResetStates();
}

void UIManager::UpdateProjection()
{
	if (m_WindowManager)
	{
		float width = static_cast<float>(m_WindowManager->WINDOW_WIDTH);
		float height = static_cast<float>(m_WindowManager->WINDOW_HEIGHT);

		m_Projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
	}
}