#pragma once
#include <string>
#include <glm/glm.hpp>

class Animal;
class GameObject;

class AnimalInteractions
{
private:
	float m_DetectionRadius = 8.0f;
	float m_DetectionTimer = 0.0f;
	float m_EatDistance = 3.0f;
	float m_HungerTimer = 0.0f;
	float m_HungerCooldown = 10.0f;

	bool m_IsChasing = false;

	GameObject* m_TargetAnimal = nullptr;
	GameObject* FindClosestAnimal(Animal* self, const std::string& targetName, float radius);

	void HandleBear(Animal* animal);
	void HandleBunny(Animal* animal);
	bool HandleSkunk(Animal* animal);

public:
	void Update(Animal* animal);

};