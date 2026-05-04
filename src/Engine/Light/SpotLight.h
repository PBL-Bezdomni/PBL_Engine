#pragma once
#include "LightSource.h"

class SpotLight: public LightSource
{
public:
	SpotLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir);
	SpotLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir, float linear, float quadratic);
	SpotLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir, float linear, float quadratic, float innCutoff, float outCutOff);
	
	void SetLightValues(Shader& shader) override;
};
