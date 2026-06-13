#pragma once
#include "../Components/Component.h"
#include <glm/glm.hpp>


class PhysicsEngine;

class RigidBody : public Component
{
public:
    RigidBody();

    void PrepareInit(const glm::vec3& halfExtents = glm::vec3(1.0, 1.0, 1.0), bool isStatic = false, bool isTrigger = false);
    void Init(const glm::vec3& halfExtents = glm::vec3(1.0, 1.0, 1.0), bool isStatic = false, bool isTrigger = false);

    void Update();

    unsigned int GetBodyID() const { return m_BodyID; }

    glm::vec3 GetLinearVelocity();

    void SetLinearVelocity(const glm::vec3& velocity);
    void SetAngularVelocity(const glm::vec3& velocity);

    void Teleport(const glm::vec3& newWorldPosition);
    void RequestTeleport(const glm::vec3& newWorldPosition);

    void MoveKinematic(const glm::vec3& newWorldPosition, const glm::quat& newWorldRotation);

    void SetRotation(const glm::vec3& eulerAnglesDegrees);

    glm::vec3 GetPosition();

    void SetHitboxSize(const glm::vec3& newHalfExtents);
    glm::vec3 GetHitBoxSize();
    bool GetIsStatic();
    bool GetIsTrigger();

private:
    glm::vec3 m_HalfExtents;
    bool m_IsStatic;
    bool m_IsTrigger;
    
    PhysicsEngine* m_PhysicsEngine = nullptr;
    unsigned int m_BodyID = 0xffffffff;
    bool m_Initialized = false;
};