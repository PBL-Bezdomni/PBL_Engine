#pragma once
#include "LightSource.h"

class DirectionalLight: public LightSource
{
public:
	DirectionalLight(shared_ptr<Camera> camera, Transform* transform, glm::vec3 dir);
	
	void SetLightValues(Shader& shader) override;
};
