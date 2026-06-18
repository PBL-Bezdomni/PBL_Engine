#pragma once
#include <GLFW/glfw3.h>

#include "Engine/WindowManager.h"
#include "Texture.h"
#include "Shader.h"
#include <UI/FreeType.h>

class AssetManager;
class PhysicsEngine;
class WindowManager;

struct UIPanel
{
	glm::vec2 Position;
	glm::vec2 Size;
	unsigned int TextureID;
	bool HasTexture = true;

	std::wstring Text = L"";
	float TextScale = 1.0f;
	glm::vec3 TextColor = glm::vec3(1.0f);

	bool HasIcon = false;
	unsigned int InconTextureID = 0;
	glm::vec2 IconSize = glm::vec2(30.0f, 30.0f);
	float IconMargin = 5.0f;
};

class UIManager {
private:
	AssetManager* m_AssetManager = nullptr;
	WindowManager* m_WindowManager = nullptr;
	FreeType m_TextRenderer;

	glm::mat4 m_Projection = glm::mat4(1.0f);

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;

	void UpdateProjection();
	void SetUIStates();
	void ResetStates();
	void InitRenderData();

public:
	void Init(AssetManager* assetManager, WindowManager* windowManager, const char* fontPath);
	void DrawSprite(Shader& shader, unsigned int textureID, glm::vec2 position, glm::vec2 size, float rotate = 0.0f);
	void DrawPanelWithText(Shader& uiShader, Shader& textShader, const UIPanel& panel);
	glm::mat4 GetProjectionMatrix() const { return m_Projection; }
};