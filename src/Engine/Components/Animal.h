#pragma once
#include "Behaviour.h"
#include <glm/glm.hpp>

class GameObject;

class Animal : public Behaviour
{
private:
    bool m_ShouldTeleport = false;
    glm::vec3 m_TeleportTarget;

public:
    void EnterTable(GameObject* table);

    void Update() override;

    bool m_IsSeated = false;
};