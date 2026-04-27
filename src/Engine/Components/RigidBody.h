#pragma once
#include "../Components/Component.h"
#include <glm/glm.hpp>


class PhysicsEngine;

class RigidBody : public Component
{
public:
    RigidBody() = default;

    void Init(PhysicsEngine* engine, const glm::vec3& halfExtents, bool isStatic);

    void Update();

    unsigned int GetBodyID() const { return m_BodyID; }

private:
    PhysicsEngine* m_PhysicsEngine = nullptr;
    unsigned int m_BodyID = 0xffffffff;
    bool m_Initialized = false;
};