#include "Bath.h"

void Bath::Awake()
{
    m_ObjectNeed = AnimalNeeds::Bath;
    AOnsenObject::Awake();
}

