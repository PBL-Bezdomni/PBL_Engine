#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Component.h"

class Transform : public Component
{
public:
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 EulerAngles = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 ModelMatrix = glm::mat4(1.0f);

	glm::mat4 GetLocalModelMatrix();
};
