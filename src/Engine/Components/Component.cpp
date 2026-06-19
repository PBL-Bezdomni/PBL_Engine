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

void Component::Awake()
{
}

void Component::Start()
{
}

void Component::Update()
{
}

void Component::DrawUpdate()
{
}

void Component::AfterDrawUpdate()
{
}

void Component::SetActive(bool active)
{
	m_IsActive = active;
}

bool Component::IsActive()
{
	return m_IsActive;
}

