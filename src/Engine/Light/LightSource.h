#pragma once
#include "Camera.h"
#include "GameObject.h"
#include "glm/glm.hpp"
#include "ShadowMap.h"

class LightSource: public Component
{
protected:
	glm::vec3 m_Color;
	glm::vec3 m_Direction;
	glm::vec3 m_ViewPosition;
	float m_Constant = 1.0f;
	float m_Linear;
	float m_Quadratic;
	// Used for spotlight.
	float m_InnerCutOff;
	float m_OuterCutOff;
	ShadowMap* shadowMap;

	weak_ptr<Camera> m_Camera;
	Transform* m_Transform;
	
public:
	LightSource(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir);
	
	virtual void SetLightValues(Shader& shader);

	void Update() override;

	// Values getters
	glm::vec3 GetColor();
	glm::vec3 GetDirection();
	float GetConstant();
	float GetLinear();
	float GetQuadratic();
	float GetInnerCutOff();
	float GetOuterCutOff();
	unsigned int GetDynamicShadowMap(GameObject& m_WorldParent, Shader& depthShader);
	unsigned int GetStaticShadowMap(GameObject& m_WorldParent, Shader& depthShader);
	glm::mat4 getLightViewMatrix();

	// Values setters
	void SetColor(glm::vec3 color);
	void SetDirection(glm::vec3 dir);
	void SetConstant(float con);
	void SetLinear(float lin);
	void SetQuadratic(float quad);
	void SetInnerCutOff(float cutOff);
	void SetOuterCutOff(float cutOff);
};
