#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.h"
#include "Engine/WindowManager.h"

class ShadowMap {
private:
	const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;
	unsigned int m_DynamicDepthMapFBO;
	unsigned int m_StaticDepthMapFBO;
	unsigned int m_DynamicDepthMap;
	unsigned int m_StaticDepthMap;
	
public:

	ShadowMap();
	unsigned int GenerateMap(GameObject& m_WorldParent, Shader& depthShader, bool isDynamic);

	glm::mat4 ConfigureShaderAndMatrices();
};