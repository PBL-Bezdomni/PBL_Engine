#pragma once
#include "Engine/Components/Behaviour.h"
#include <glm/glm.hpp>
#include <vector>

#include "ObjectSlot.h"

class Animal;
class GameObject;

class Bath : public Behaviour
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

    const char* GetScriptName() const override { return "Bath"; }
};