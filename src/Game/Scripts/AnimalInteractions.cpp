#include "AnimalInteractions.h"
#include "Engine/Time.h"
#include "Animal.h"
#include "GameObject.h"
#include "SpawnManager.h"
#include <Engine/Components/RigidBody.h>
#include <Random.h>
#include "AOnsenObject.h"

void AnimalInteractions::Update(Animal* animal)
{
	if (animal == nullptr || animal->GetGameObject() == nullptr) return;

	if (animal->m_CurrentOnsen != nullptr)
	{
		HandleOnsenInteractions(animal);
		return;
	}

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
		if (!m_IsChasing)
		{
			m_IsChasing = true;
			m_ChasingSpeed = Random::GetRandomFloat(5.0f, 9.0f);
		}

		animal->GetStateController()->RequestStateChange.Invoke(AnimalState::Chasing);

		RigidBody* rb = animal->GetGameObject()->GetComponent<RigidBody>();
		glm::vec3 currentPos = animal->GetGameObject()->transform->GetGlobalPosition();
		glm::vec3 bunnyPos = bunny->transform->GetGlobalPosition();

		glm::vec3 dir = glm::normalize(bunnyPos - currentPos);
		dir.y = 0.0f;

		if (rb != nullptr)
		{
			rb->SetLinearVelocity(dir * m_ChasingSpeed + glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
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
			m_HungerTimer = 4.0f;
		}
	}
}

void AnimalInteractions::HandleBunny(Animal* animal)
{
	GameObject* bear = FindClosestAnimal(animal, "bear", m_DetectionRadius);

	if (bear != nullptr)
	{
		if (!m_IsFleeing)
		{
			m_IsFleeing = true;
			m_FleeSpeed = Random::GetRandomFloat(5.0f, 9.0f);
			animal->SetSpeed(m_FleeSpeed);
		}

		glm::vec3 runDirection = glm::normalize(animal->GetGameObject()->transform->GetGlobalPosition() - bear->transform->GetGlobalPosition());

		glm::vec3 escapePosition = animal->GetGameObject()->transform->GetGlobalPosition() + (runDirection * 5.0f);
		escapePosition.y = animal->GetGameObject()->transform->Position.y;
		animal->SetTargetPosition(escapePosition);
		
	}
	else
	{
		if (m_IsFleeing)
		{
			m_IsFleeing = false;
		}
	}
}

bool AnimalInteractions::HandleSkunk(Animal* animal)
{
	GameObject* skunk = FindClosestAnimal(animal, "skunk", m_DetectionRadius*1.5f);

	if (skunk != nullptr)
	{
		glm::vec3 runDirection = glm::normalize(animal->GetGameObject()->transform->GetGlobalPosition() - skunk->transform->GetGlobalPosition());

		glm::vec3 escapePosition = animal->GetGameObject()->transform->GetGlobalPosition() + (runDirection * 5.0f);
		escapePosition.y = animal->GetGameObject()->transform->Position.y;
		animal->SetTargetPosition(escapePosition);
		m_IsChasing = false;
		animal->SetSpeed(7.0);
		animal->GetStateController()->RequestStateChange.Invoke(AnimalState::Walking);
		return true;
	}
	return false;
}

void AnimalInteractions::HandleOnsenInteractions(Animal* animal)
{
	std::string name = animal->GetGameObject()->Name;

	if (name.find("bear") != std::string::npos)
	{
		Animal* bunny = nullptr;
		for (Animal* other : animal->m_CurrentOnsen->GetAnimalsInObject())
		{
			if (other != animal && other->GetGameObject()->Name.find("bunny") != std::string::npos)
			{
				bunny = other;
				break;
			}
		}

		if (bunny != nullptr)
		{
			if (!m_IsOnsenTimerActive)
			{
				m_IsOnsenTimerActive = true;
				m_OnsenTimer = Random::GetRandomFloat(3.0f, 7.0f);
			}

			m_OnsenTimer -= Time::GetDeltaTime();
			if (m_OnsenTimer <= 0.0f)
			{
				animal->m_CurrentOnsen->RemoveAnimal(bunny);
				if (SpawnManager::Instance != nullptr)
				{
					SpawnManager::Instance->DespawnAnimal(bunny->GetGameObject());
				}
				m_IsOnsenTimerActive = false;
			}
		}
		else
		{
			m_IsOnsenTimerActive = false;
		}
	}

	else if (name.find("skunk") != std::string::npos)
	{
		std::vector<Animal*> victims;
		for (Animal* other : animal->m_CurrentOnsen->GetAnimalsInObject())
		{
			if (other != animal && other->GetGameObject()->Name.find("skunk") == std::string::npos)
			{
				victims.push_back(other);
			}
		}

		if (!victims.empty())
		{
			if (!m_IsOnsenTimerActive)
			{
				m_IsOnsenTimerActive = true;
				m_OnsenTimer = Random::GetRandomFloat(3.0f, 6.0f);
			}

			m_OnsenTimer -= Time::GetDeltaTime();
			if (m_OnsenTimer <= 0.0f)
			{
				for (Animal* victim : victims)
				{
					animal->m_CurrentOnsen->RemoveAnimal(victim);
					
					
					glm::vec3 victimPos = victim->GetGameObject()->transform->GetGlobalPosition();
					float thrustZ = (victimPos.z < -30.0f) ? 1.0f : -1.0f;
					glm::vec3 launchDir = glm::vec3(0.0f, 0.0f, thrustZ);

					glm::vec3 launchVelocity = launchDir * 15.0f + glm::vec3(0.0f, 12.0f, 0.0f);

					RigidBody* rb = victim->GetGameObject()->GetComponent<RigidBody>();
					if (rb != nullptr)
					{
						rb->SetLinearVelocity(launchVelocity);

						float spinX = Random::GetRandomFloat(-15.0f, 15.0f);
						float spinY = Random::GetRandomFloat(-15.0f, 15.0f);
						float spinZ = Random::GetRandomFloat(-15.0f, 15.0f);

						rb->SetAngularVelocity(glm::vec3(spinX, spinY, spinZ));
					}

					victim->GetStateController()->RequestStateChange.Invoke(AnimalState::Throw);

					victim->m_WaitTime = 1.5f;
					victim->m_CurrentWaitTime = 0.0f;
					victim->m_StuckTimer = 0.0f;
				}
				
				m_IsOnsenTimerActive = false;
			}
		}
		else
		{
			m_IsOnsenTimerActive = false;
		}
	}

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