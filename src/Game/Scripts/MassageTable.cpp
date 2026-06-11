#include "MassageTable.h"
#include "Animal.h"
#include "TutorialArrow.h"

void MassageTable::Awake()
{
    m_ObjectNeed = AnimalNeeds::Massage;
    m_MaxSlots = 1;
    m_SlotsPos[0] = glm::vec3(0, -4.f, 0);
    AOnsenObject::Awake();
    m_TutorialArrow->SetStartPosition(glm::vec3(-20.f, 0.f, 0.f));
}
