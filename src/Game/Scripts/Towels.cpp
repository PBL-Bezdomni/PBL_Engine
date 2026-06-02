#include "Towels.h"
#include "Animal.h"
#include "GameObject.h"
#include "ObjectSlot.h"

void Towels::Awake()
{
	Behaviour::Awake();

	m_Slots.resize(m_MaxSlots);

	m_Slots[0].LocalOffset = glm::vec3(-3.0f, 2.5f, -3.0f);
	m_Slots[1].LocalOffset = glm::vec3(3.0f, 2.5f, -3.0f);
	m_Slots[2].LocalOffset = glm::vec3(-3.0f, 2.5f, 3.0f);
	m_Slots[3].LocalOffset = glm::vec3(3.0f, 2.5f, 3.0f);
}

void Towels::OnTriggerEnter(GameObject* other)
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
		animal->StartFulfillingNeed(AnimalNeeds::Towels);
	}
	else
	{
		//bath full
	}
}

void Towels::OnTriggerExit(GameObject* other)
{

}