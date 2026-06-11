#include "Towels.h"
#include "TutorialArrow.h"

void Towels::Awake()
{
	m_ObjectNeed = AnimalNeeds::Towels;
	AOnsenObject::Awake();
	m_TutorialArrow->SetStartPosition(glm::vec3(-30.f, 0.f, 0.f));
}
