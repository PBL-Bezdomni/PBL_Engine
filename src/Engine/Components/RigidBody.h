#pragma once
#include "../Components/Component.h"
#include <glm/glm.hpp>


class PhysicsEngine;

class RigidBody : public Component
{
public:
    RigidBody() = default;

    void Init(const glm::vec3& halfExtents = glm::vec3(1.0, 1.0, 1.0), bool isStatic = false, bool isTrigger = false);

    void Update();

    unsigned int GetBodyID() const { return m_BodyID; }

    glm::vec3 GetLinearVelocity();

    void SetLinearVelocity(const glm::vec3& velocity);

private:
    PhysicsEngine* m_PhysicsEngine = nullptr;
    unsigned int m_BodyID = 0xffffffff;
    bool m_Initialized = false;
};