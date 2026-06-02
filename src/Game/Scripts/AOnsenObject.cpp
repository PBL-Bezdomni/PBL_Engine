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

	int freeSlotIndex = -1;
	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (!m_Slots[i].IsOccupied)
		{
			freeSlotIndex = i;
			break;
		}
	}

	if (freeSlotIndex != -1)
	{
		m_Slots[freeSlotIndex].IsOccupied = true;
		m_Slots[freeSlotIndex].OccupyingAnimal = animal;

		glm::vec3 tubWorldPos = m_Owner->GetWorldPosition();
		glm::vec3 targetPos = tubWorldPos + m_Slots[freeSlotIndex].LocalOffset;

		animal->EnterPosition(targetPos);
		animal->StartFulfillingNeed(AnimalNeeds::Bath);
	}
	else
	{
		//bath full
	}
}

void AOnsenObject::OnTriggerExit(GameObject* other)
{

}