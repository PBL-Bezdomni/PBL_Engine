#include "Component.h"
#include "GameObject.h"

GameObject* Component::GetOwner()
{
	return m_Owner;
}

void Component::SetOwner(GameObject* gameObject)
{
	m_Owner = gameObject;
}

