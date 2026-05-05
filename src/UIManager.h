#pragma once
#include <GLFW/glfw3.h>

#include "Engine/WindowManager.h"
#include "Texture.h"
#include "Shader.h"

class AssetManager;
class PhysicsEngine;
class DebugManager;
class WindowManager;

class UIManager {
private:
	AssetManager* m_AssetManager = nullptr;
	WindowManager* m_WindowManager = nullptr;

	glm::mat4 m_Projection = glm::mat4(1.0f);

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;

	void UpdateProjection();
	void SetUIStates();
	void ResetStates();
	void InitRenderData();

public:
	void Init(AssetManager* assetManager, WindowManager* windowManager);
	void DrawSprite(Shader& shader, unsigned int textureID, glm::vec2 position, glm::vec2 size, float rotate = 0.0f);
	glm::mat4 GetProjectionMatrix() const { return m_Projection; }
};