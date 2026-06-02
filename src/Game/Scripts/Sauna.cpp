#include "Sauna.h"

void Sauna::Awake()
{
	m_ObjectNeed = AnimalNeeds::Sauna;
	AOnsenObject::Awake();
}