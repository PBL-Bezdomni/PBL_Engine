#include "Towels.h"
#include "TutorialArrow.h"

void Towels::Awake()
{
	m_ObjectNeed = AnimalNeeds::Towels;
	AOnsenObject::Awake();
	m_TutorialArrow->SetStartPosition(glm::vec3(-30.f, 0.f, 0.f));

	m_IsSquareIndicator = true;
	m_IndicatorScale = 40.0f;
	m_IndicatorOffset = glm::vec3(0.0f, -18.0f, 0.0f);
	m_Indicator->transform->Scale = glm::vec3(m_IndicatorScale);
	m_Indicator->transform->Position = m_IndicatorOffset;
}
