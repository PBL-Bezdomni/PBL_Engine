#pragma once
#include "Behaviour.h"
#include <glm/glm.hpp>
#include <vector>

class GameObject;

enum class AnimalNeeds
{
    Bath,
    Towels,
    Sauna,
    Massage
};

class Animal : public Behaviour
{
private:
    int m_minNeeds = 1;
    int m_maxNeeds = 4;

    int m_numberOfNeeds = 1;

    bool m_ShouldTeleport = false;
    glm::vec3 m_TeleportTarget;

public:
    std::vector<AnimalNeeds> m_RequiredServices;

    bool m_IsSeated = false;

    void Init();

    void EnterTable(GameObject* table);

    void Update() override;
};