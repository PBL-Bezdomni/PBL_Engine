#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/WindowManager.h"
#include "Shader.h"

class CelShading {
private:
	unsigned int m_PostProcessFBO;
	unsigned int m_ScreenColorTex;
	unsigned int m_ScreenNormalTex;
	unsigned int m_ScreenDepthTex;
	unsigned int m_ScreenGlowTex;
	unsigned int m_QuadVAO = 0, m_QuadVBO;
	int windowWidth, windowHeight;
public:
	CelShading(GLFWwindow* window);
	void FBOInit();
	void RenderQuad(Shader& shader);
	void CheckScreenSize(GLFWwindow* window);
};