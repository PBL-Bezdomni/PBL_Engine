#pragma once
#include "LightSource.h"

class PointLight: public LightSource
{
public:
	PointLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir);
	PointLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir, float linear, float quadratic);
	
	void SetLightValues(Shader& shader) override;
};
