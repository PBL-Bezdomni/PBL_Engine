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
	glm::vec3 GetGlobalScale() const;
	glm::vec3 GetGlobalPosition() const;

	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForward() const;
	glm::vec3 GetBackward() const;
	
	void Awake() override;
	void Start() override;
	void Update() override;
};
