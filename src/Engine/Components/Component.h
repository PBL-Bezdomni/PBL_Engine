#pragma once
// #include "GameObject.h"

class GameObject;

class Component
{
protected:
	GameObject* m_Owner = nullptr;
	
public:
	virtual ~Component() = default;

	GameObject* GetOwner();
	void SetOwner(GameObject* gameObject);
	
	virtual bool AllowMultiple() const { return false; }
};
