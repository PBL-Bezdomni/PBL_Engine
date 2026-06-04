#include "ShadowMap.h"

ShadowMap::ShadowMap() {
	glGenFramebuffers(1, &m_DynamicDepthMapFBO);
	glGenTextures(1, &m_DynamicDepthMap);
	glBindTexture(GL_TEXTURE_2D, m_DynamicDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor1[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor1);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_DynamicDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DynamicDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// I don't like this bulk of code repeating but I didn't figure out how to move it to function.
	glGenFramebuffers(1, &m_StaticDepthMapFBO);
	glGenTextures(1, &m_StaticDepthMap);
	glBindTexture(GL_TEXTURE_2D, m_StaticDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor2[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor2);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_StaticDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_StaticDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int ShadowMap::GenerateMap(GameObject& m_WorldParent, Shader& depthShader, bool isDynamic) {
	glm::mat4 lightSpaceMatrix = ConfigureShaderAndMatrices();
	depthShader.Use();
	depthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	if (isDynamic)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_DynamicDepthMapFBO);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_StaticDepthMapFBO);
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	m_WorldParent.DrawSekfAndChildShadow(&depthShader, isDynamic);
	//glCullFace(GL_BACK);
	 //glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (isDynamic)
	{
		return m_DynamicDepthMap;
	}
	else
	{
		return m_StaticDepthMap;
	}
}

glm::mat4 ShadowMap::ConfigureShaderAndMatrices() {
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.1f, far_plane = 100.0f;
	lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

	glm::vec3 sceneCenter = glm::vec3(0.0f, 0.0f, -30.0f);
	glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
	glm::vec3 lightPos = sceneCenter - (lightDir * 60.0f);

	lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	return lightSpaceMatrix;
}