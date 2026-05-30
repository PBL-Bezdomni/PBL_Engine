#pragma once
#include "Engine/Components/Behaviour.h"
#include <glm/glm.hpp>
#include <vector>

class Animal;
class GameObject;

struct BathSlot
{
    bool IsOccupied = false;
    Animal* OccupyingAnimal = nullptr;
    glm::vec3 LocalOffset;
};

class Bath : public Behaviour
{
private:
    int m_MaxSlots = 4;
    std::vector<BathSlot> m_Slots;

    bool m_IsOccupied = false;
    GameObject* m_OccupyingAnimal = nullptr;

public:
    void Awake() override;

    void OnTriggerEnter(GameObject* other) override;
    void OnTriggerExit(GameObject* other) override;

    const char* GetScriptName() const override { return "Bath"; }
};