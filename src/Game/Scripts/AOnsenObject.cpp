#include "AOnsenObject.h"
#include "Animal.h"
#include "TutorialArrow.h"
#include "glm/vec3.hpp"

void AOnsenObject::Awake()
{
	Behaviour::Awake();

	for (int i = 0; i < m_MaxSlots; i++)
	{
		m_Slots.push_back(ObjectSlot());
		m_Slots[i].LocalOffset = m_SlotsPos[i];
	}

	m_TutorialArrow = m_Owner->AddComponent<TutorialArrow>();
}

void AOnsenObject::OnTriggerEnter(GameObject* other)
{
	Animal* animal = other->GetComponent<Animal>();
	if (animal == nullptr) return;
	if (!animal->m_WasDroppedByPlayer) return;
	animal->m_WasDroppedByPlayer = false;
	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (m_Slots[i].OccupyingAnimal == animal)
		{
			return;
		}
	}
	
	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (!m_Slots[i].IsOccupied)
		{
			m_Slots[i].IsOccupied = true;
			m_Slots[i].OccupyingAnimal = animal;

			glm::vec3 tubWorldPos = m_Owner->GetWorldPosition();
			glm::vec3 targetPos = tubWorldPos + m_Slots[i].LocalOffset;

			animal->EnterPosition(targetPos);
			animal->StartFulfillingNeed(m_ObjectNeed);
			animal->m_CurrentOnsen = this;
			break;
		}
	}
}

void AOnsenObject::OnTriggerExit(GameObject* other)
{
	// Animal* animal = other->GetComponent<Animal>();
	// if (animal == nullptr) return;
	//
	// for (int i = 0; i < m_MaxSlots; i++)
	// {
	// 	if (m_Slots[i].OccupyingAnimal == animal)
	// 	{
	// 		m_Slots[i].IsOccupied = false;
	// 		m_Slots[i].OccupyingAnimal = nullptr;
	//
	// 		animal->StopFulfillingNeed();
	// 		animal->m_CurrentOnsen = nullptr;
	// 		return;
	// 	}
	// }
}

void AOnsenObject::RemoveAnimal(Animal* animal)
{
	if (animal == nullptr) return;
	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (m_Slots[i].OccupyingAnimal == animal)
		{
			m_Slots[i].IsOccupied = false;
			m_Slots[i].OccupyingAnimal = nullptr;
			animal->m_CurrentOnsen = nullptr;
			animal->StopFulfillingNeed();
			return;
		}
	}
}

Animal* AOnsenObject::GetAnimalForPlayer(glm::vec3 rayStart, glm::vec3 rayDir)
{
	Animal* closestAnimal = nullptr;
	float minDistance = 999.0f;

	glm::vec3 worldPos = m_Owner->GetWorldPosition();

	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (m_Slots[i].IsOccupied && m_Slots[i].OccupyingAnimal != nullptr)
		{
			glm::vec3 animalPos = worldPos + m_Slots[i].LocalOffset;
			glm::vec3 wek = animalPos - rayStart;
			
			float dot = glm::dot(wek, rayDir);
			if (dot < 0.0f) continue;

			glm::vec3 projection = rayStart + rayDir * dot;

			float distToRay = glm::length(animalPos - projection);

			if (distToRay < minDistance)
			{
				minDistance = distToRay;
				closestAnimal = m_Slots[i].OccupyingAnimal;
			}
		}
	}
	return closestAnimal;
}

AnimalNeeds AOnsenObject::GetNeed()
{
	return m_ObjectNeed;
}

TutorialArrow* AOnsenObject::GetTutorialArrow()
{
	return m_TutorialArrow;
}
