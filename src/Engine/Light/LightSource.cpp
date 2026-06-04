#include "LightSource.h"

LightSource::LightSource(shared_ptr<Camera> camera, Transform* transform,  glm::vec3 dir)
{
	m_Camera = camera;
	m_Transform = transform;
	m_Direction = dir;
	m_ViewPosition = m_Camera.lock()->GetPosition();
	m_Color = glm::vec3(1.f, 1.f, 1.f);

	m_Linear = 0.f;
	m_Quadratic = 0.f;
	m_InnerCutOff = 0.f;
	m_OuterCutOff = 0.f;

	shadowMap = new ShadowMap();
}

void LightSource::SetLightValues(Shader& shader)
{
	shader.SetVec3("viewPos", m_Camera.lock()->GetPosition());
}

void LightSource::Update()
{
}

// Getters

glm::vec3 LightSource::GetColor()
{
	return m_Color;
}

glm::vec3 LightSource::GetDirection()
{
	return m_Direction;
}

float LightSource::GetConstant()
{
	return m_Constant;
}

float LightSource::GetLinear()
{
	return m_Linear;
}

float LightSource::GetQuadratic()
{
	return m_Quadratic;
}

float LightSource::GetInnerCutOff()
{
	return m_InnerCutOff;
}

float LightSource::GetOuterCutOff()
{
	return m_OuterCutOff;
}

unsigned int LightSource::GetDynamicShadowMap(GameObject& m_WorldParent, Shader& depthShader) {
	return shadowMap->GenerateMap(m_WorldParent, depthShader, true);
}

unsigned int LightSource::GetStaticShadowMap(GameObject& m_WorldParent, Shader& depthShader)
{
	return shadowMap->GenerateMap(m_WorldParent, depthShader, false);
}

glm::mat4 LightSource::getLightViewMatrix() {
	return shadowMap->ConfigureShaderAndMatrices();
}

// Setters

void LightSource::SetColor(glm::vec3 color)
{
	m_Color = color;
}

void LightSource::SetDirection(glm::vec3 dir)
{
	m_Direction = dir;
}

void LightSource::SetConstant(float con)
{
	m_Constant = con;
}

void LightSource::SetLinear(float lin)
{
	m_Linear = lin;
}

void LightSource::SetQuadratic(float quad)
{
	m_Quadratic = quad;
}

void LightSource::SetInnerCutOff(float cutOff)
{
	m_InnerCutOff = cutOff;
}

void LightSource::SetOuterCutOff(float cutOff)
{
	m_OuterCutOff = cutOff;
}


