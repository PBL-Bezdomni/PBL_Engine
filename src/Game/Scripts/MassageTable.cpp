#include "MassageTable.h"
#include "Animal.h"
#include "GameObject.h"

void MassageTable::OnTriggerEnter(GameObject* other)
{
    if (m_IsOccupied) return;

    Animal* animal = other->GetComponent<Animal>();
    if (animal != nullptr)
    {
        m_IsOccupied = true;
        m_OccupyingAnimal = other;

        animal->EnterTable(m_Owner);
    }
}