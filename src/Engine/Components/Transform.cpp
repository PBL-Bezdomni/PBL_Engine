#include "Transform.h"

void Transform::Awake()
{
	Component::Awake();
}
void Transform::Start()
{
	// int i = 0;
	Component::Start();
}
void Transform::Update()
{
	Component::Update();
}

glm::mat4 Transform::GetLocalModelMatrix()
{
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(EulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(EulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(EulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Y * X * Z
	/*const glm::mat4 rotationMatrix = transformY * transformX * transformZ;*/

	// translation * rotation * scale (also know as TRS matrix)
	return glm::translate(glm::mat4(1.0f), Position) * rotationMatrix * glm::scale(glm::mat4(1.0f), Scale);//glm::translate(glm::mat4(1.0f), transform.pos) * 
}

glm::vec3 Transform::GetGlobalScale() const
{
	return { glm::length(GetRight()), glm::length(GetUp()), glm::length(GetBackward()) };
}

glm::vec3 Transform::GetGlobalPosition() const
{
	return ModelMatrix[3];
}

glm::vec3 Transform::GetRight() const
{
	return ModelMatrix[0];
}

glm::vec3 Transform::GetUp() const
{
	return ModelMatrix[1];
}

glm::vec3 Transform::GetForward() const
{
	return -ModelMatrix[2];
}

glm::vec3 Transform::GetBackward() const
{
	return ModelMatrix[2];
}