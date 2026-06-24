#include "AnimalInteractions.h"
#include "Engine/Time.h"
#include "Animal.h"
#include "GameObject.h"
#include "SpawnManager.h"
#include <Engine/Components/RigidBody.h>

void AnimalInteractions::Update(Animal* animal)
{
	if (animal == nullptr || animal->GetGameObject() == nullptr) return;

	std::string name = animal->GetGameObject()->Name;

	if (name.find("skunk") == std::string::npos)
	{
		if (HandleSkunk(animal))
		{
			return;
		}
	}

	if (name.find("bear") != std::string::npos)
	{
		HandleBear(animal);
	}
	else if (name.find("bunny") != std::string::npos)
	{
		HandleBunny(animal);
	}

}

void AnimalInteractions::HandleBear(Animal* animal)
{
	if (m_HungerTimer > 0.0f)
	{
		m_HungerTimer -= Time::GetDeltaTime();
		m_IsChasing = false;
		return;
	}

	GameObject* bunny = FindClosestAnimal(animal, "bunny", m_DetectionRadius);
	if (bunny != nullptr)
	{
		animal->GetStateController()->RequestStateChange.Invoke(AnimalState::Chasing);
		m_IsChasing = true;

		RigidBody* rb = animal->GetGameObject()->GetComponent<RigidBody>();
		glm::vec3 currentPos = animal->GetGameObject()->transform->GetGlobalPosition();
		glm::vec3 bunnyPos = bunny->transform->GetGlobalPosition();

		glm::vec3 dir = glm::normalize(bunnyPos - currentPos);
		dir.y = 0.0f;

		if (rb != nullptr)
		{
			rb->SetLinearVelocity(dir * 5.3f + glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
			float targetAngle = glm::degrees(atan2(dir.x, dir.z));
			rb->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));
		}

		float dist = glm::length(bunnyPos - currentPos);
		if (dist <= m_EatDistance)
		{
			if (SpawnManager::Instance != nullptr)
			{
				animal->GetStateController()->RequestStateChange.Invoke(AnimalState::Eating);
				SpawnManager::Instance->DespawnAnimal(bunny);
				m_IsChasing = false;
				m_HungerTimer = m_HungerCooldown;
				animal->ForceNewTargetPosition();
			}
		}
	}
	else
	{
		if (m_IsChasing)
		{
			m_IsChasing = false;
			animal->GetStateController()->RequestStateChange.Invoke(AnimalState::Idle);
		}
	}
}

void AnimalInteractions::HandleBunny(Animal* animal)
{
	GameObject* bear = FindClosestAnimal(animal, "bear", m_DetectionRadius);

	if (bear != nullptr)
	{
		glm::vec3 runDirection = glm::normalize(animal->GetGameObject()->transform->GetGlobalPosition() - bear->transform->GetGlobalPosition());

		glm::vec3 escapePosition = animal->GetGameObject()->transform->GetGlobalPosition() + (runDirection * 5.0f);
		escapePosition.y = animal->GetGameObject()->transform->Position.y;
		animal->SetTargetPosition(escapePosition);
	}
}

bool AnimalInteractions::HandleSkunk(Animal* animal)
{
	GameObject* skunk = FindClosestAnimal(animal, "skunk", m_DetectionRadius/2);

	if (skunk != nullptr)
	{
		glm::vec3 runDirection = glm::normalize(animal->GetGameObject()->transform->GetGlobalPosition() - skunk->transform->GetGlobalPosition());

		glm::vec3 escapePosition = animal->GetGameObject()->transform->GetGlobalPosition() + (runDirection * 5.0f);
		escapePosition.y = animal->GetGameObject()->transform->Position.y;
		animal->SetTargetPosition(escapePosition);
		m_IsChasing = false;

		return true;
	}
	return false;
}

GameObject* AnimalInteractions::FindClosestAnimal(Animal* self, const std::string& targetName, float radius)
{
	if (self->GetGameObject()->Parent == nullptr) return nullptr;

	GameObject* closestObj = nullptr;
	float minDistance = radius;

	for (GameObject* child : self->GetGameObject()->Parent->Children)
	{
		if (child != self->GetGameObject() && child->Name.find(targetName) != std::string::npos && child->transform->Position.y > -10.0f)
		{
			float dist = glm::length(child->transform->GetGlobalPosition() - self->GetGameObject()->transform->GetGlobalPosition());
			if (dist <= minDistance)
			{
				minDistance = dist;
				closestObj = child;
			}
		}
	}
	return closestObj;
}