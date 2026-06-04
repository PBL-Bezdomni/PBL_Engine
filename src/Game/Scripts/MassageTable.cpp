#include "MassageTable.h"
#include "Animal.h"

void MassageTable::Awake()
{
    m_ObjectNeed = AnimalNeeds::Massage;
    m_MaxSlots = 1;
    m_SlotsPos[0] = glm::vec3(0, 2.5f, 0);
    AOnsenObject::Awake();
}
