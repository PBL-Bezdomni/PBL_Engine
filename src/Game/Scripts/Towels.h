#pragma once
#include <vector>

#include "Engine/Components/Behaviour.h"

class ObjectSlot;

class Towels: public Behaviour
{
private:
	int m_MaxSlots = 4;
	std::vector<ObjectSlot> m_Slots;

	bool m_IsOccupied = false;
	GameObject* m_OccupyingAnimal = nullptr;
public:
	void Awake() override;

	void OnTriggerEnter(GameObject* other) override;
	void OnTriggerExit(GameObject* other) override;
	
	const char* GetScriptName() const override { return "Towels"; }
};
