#include "Engine/Engine.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

void RigidBody::Init(const glm::vec3& halfExtents, bool isStatic, bool isTrigger)
{
    m_PhysicsEngine = &Engine::GetInstance().GetPhysicsEngine();

    glm::mat4 currentMatrix = m_Owner->transform->ModelMatrix;
    glm::vec3 worldPos = glm::vec3(currentMatrix[3]);

    glm::mat4 rotMatrix = currentMatrix;
    rotMatrix[0] = glm::normalize(rotMatrix[0]);
    rotMatrix[1] = glm::normalize(rotMatrix[1]);
    rotMatrix[2] = glm::normalize(rotMatrix[2]);

    glm::quat worldRot = glm::quat_cast(rotMatrix);
    worldRot = glm::normalize(worldRot);

    JPH::BodyInterface& bodyInterface = m_PhysicsEngine->GetSystem()->GetBodyInterface();

    JPH::BodyID id = m_PhysicsEngine->CreateBox(worldPos, worldRot, halfExtents, isStatic);
    m_BodyID = id.GetIndexAndSequenceNumber();

    if (isTrigger)
    {
        bodyInterface.SetIsSensor(id, true);
    }

    bodyInterface.SetUserData(id, reinterpret_cast<uint64_t>(m_Owner));

    m_Initialized = true;
}

void RigidBody::Update()
{
    if (!m_Initialized || !m_PhysicsEngine) return;

    JPH::BodyID realID(m_BodyID);

    glm::mat4 physicsMatrix = m_PhysicsEngine->GetMatrix(realID);

    glm::mat4 localMatrix = physicsMatrix;

    if (m_Owner->Parent != nullptr)
    {
        localMatrix = glm::inverse(m_Owner->Parent->transform->ModelMatrix) * physicsMatrix;
    }

    m_Owner->transform->Position = glm::vec3(localMatrix[3]);

    localMatrix[0] = glm::normalize(localMatrix[0]);
    localMatrix[1] = glm::normalize(localMatrix[1]);
    localMatrix[2] = glm::normalize(localMatrix[2]);

    glm::quat localRot = glm::quat_cast(localMatrix);
    m_Owner->transform->EulerAngles = glm::degrees(glm::eulerAngles(localRot));
}

glm::vec3 RigidBody::GetLinearVelocity() {
    JPH::Vec3 v = m_PhysicsEngine->GetSystem()->GetBodyInterface().GetLinearVelocity(JPH::BodyID(m_BodyID));

    return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
}

void RigidBody::SetLinearVelocity(const glm::vec3& velocity) {
    m_PhysicsEngine->GetSystem()->GetBodyInterface().SetLinearVelocity(
        JPH::BodyID(m_BodyID),
        JPH::Vec3(velocity.x, velocity.y, velocity.z)
    );
}