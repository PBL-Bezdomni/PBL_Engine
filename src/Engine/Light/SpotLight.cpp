#include "SpotLight.h"

SpotLight::SpotLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir) : LightSource(camera, transform, dir)
{
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	m_InnerCutOff = 24.5f;
	m_OuterCutOff = 30.5f;
}

SpotLight::SpotLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir, float linear, float quadratic) : LightSource(camera, transform, dir)
{
	m_Linear = linear;
	m_Quadratic = quadratic;

	m_InnerCutOff = 24.5f;
	m_OuterCutOff = 30.5f;
}

SpotLight::SpotLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir, float linear, float quadratic, float innCutoff, float outCutOff) : LightSource(camera, transform, dir)
{
	m_Linear = linear;
	m_Quadratic = quadratic;

	m_InnerCutOff = innCutoff;
	m_OuterCutOff = outCutOff;
}


void SpotLight::SetLightValues(Shader& shader)
{
	LightSource::SetLightValues(shader);
	shader.SetBool("useSpotLight", m_IsActive);
	shader.SetVec3("spotLight.color", m_Color);
	shader.SetVec3("spotLight.position", m_Transform->Position);
	shader.SetVec3("spotLight.direction", m_Direction);
	shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(m_InnerCutOff)));
	shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(m_OuterCutOff)));
	shader.SetFloat("spotLight.constant", m_Constant);
	shader.SetFloat("spotLight.linear", m_Linear);
	shader.SetFloat("spotLight.quadratic", m_Quadratic);
}
