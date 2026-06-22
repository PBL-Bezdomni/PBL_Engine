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

    m_IsSquareIndicator = true;
    m_IndicatorScale = 20.0f;
    m_IndicatorOffset = glm::vec3(0.0f, -18.0f, 0.0f);
    m_Indicator->transform->Scale = glm::vec3(m_IndicatorScale);
    m_Indicator->transform->Position = m_IndicatorOffset;
}
