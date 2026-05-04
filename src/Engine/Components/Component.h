#pragma once
// #include "GameObject.h"

class GameObject;

class Component
{
protected:
	GameObject* m_Owner = nullptr;
	bool m_IsActive = true;
	
public:
	virtual ~Component() = default;

	GameObject* GetOwner();
	void SetOwner(GameObject* gameObject);
	
	virtual bool AllowMultiple() const { return false; }
	virtual void Update();

	void SetActive(bool active);
	bool IsActive();
};
