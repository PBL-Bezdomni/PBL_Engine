#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir) : LightSource(camera, transform, dir)
{
}

void DirectionalLight::SetLightValues(Shader& shader)
{
	LightSource::SetLightValues(shader);
	shader.SetBool("useDirLight", m_IsActive);
	shader.SetVec3("dirLight.color", m_Color);
	shader.SetVec3("dirLight.direction", m_Direction);
}
