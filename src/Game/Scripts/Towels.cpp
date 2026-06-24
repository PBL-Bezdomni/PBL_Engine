#include "Towels.h"
#include "TutorialArrow.h"

void Towels::Awake()
{
	m_ObjectNeed = AnimalNeeds::Towels;

	m_IsSquareIndicator = true;
	m_IndicatorScale = 40.0f;
	m_IndicatorOffset = glm::vec3(0.0f, -18.0f, 0.0f);

	m_IconTexturePath = "res/textures/UI/objects/towel.png";
	m_IconScale = glm::vec3(3.0f);
	m_IconOffset = glm::vec3(0.0f, 50.0f, -10.0f);

	AOnsenObject::Awake();

	m_TutorialArrow->SetStartPosition(glm::vec3(-30.f, 0.f, 0.f));
}
