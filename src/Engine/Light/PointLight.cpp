#include "PointLight.h"

PointLight::PointLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir) : LightSource(camera, transform, dir)
{
	m_Linear = 0.0014f;
	m_Quadratic = 0.000007f;
}

PointLight::PointLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir, float linear, float quadratic) : LightSource(camera, transform, dir)
{
	m_Linear = linear;
	m_Quadratic = quadratic;
}

void PointLight::SetLightValues(Shader& shader)
{
	LightSource::SetLightValues(shader);
	shader.SetBool("usePointLight", m_IsActive);
	shader.SetVec3("pointLight.color", m_Color);
	shader.SetVec3("pointLight.position", m_Transform->Position);
	shader.SetFloat("pointLight.constant", m_Constant);
	shader.SetFloat("pointLight.linear", m_Linear);
	shader.SetFloat("pointLight.quadratic", m_Quadratic);
}
