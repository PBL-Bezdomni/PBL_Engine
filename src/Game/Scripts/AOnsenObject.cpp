#include "AOnsenObject.h"
#include "Animal.h"
#include "glm/vec3.hpp"

void AOnsenObject::Awake()
{
	Behaviour::Awake();

	for (int i = 0; i < m_MaxSlots; i++)
	{
		m_Slots.push_back(ObjectSlot());
		m_Slots[i].LocalOffset = m_SlotsPos[i];
	}
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
			break;
		}
	}
}

void AOnsenObject::OnTriggerExit(GameObject* other)
{
	Animal* animal = other->GetComponent<Animal>();
	if (animal == nullptr) return;

	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (m_Slots[i].OccupyingAnimal == animal)
		{
			m_Slots[i].IsOccupied = false;
			m_Slots[i].OccupyingAnimal = nullptr;

			animal->StopFulfillingNeed();
			return;
		}
	}
}