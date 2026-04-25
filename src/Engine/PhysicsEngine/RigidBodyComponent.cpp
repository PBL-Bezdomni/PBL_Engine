#include "RigidBodyComponent.h"
#include "GameObject.h"
#include "PhysicsEngine.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

void RigidBodyComponent::Init(PhysicsEngine* engine, const glm::vec3& halfExtents, bool isStatic)
{
    m_PhysicsEngine = engine;

    glm::mat4 currentMatrix = m_Owner->transform->ModelMatrix;
    glm::vec3 worldPos = glm::vec3(currentMatrix[3]);
    glm::quat worldRot = glm::quat_cast(currentMatrix);

    JPH::BodyID id = m_PhysicsEngine->CreateBox(worldPos, worldRot, halfExtents, isStatic);
    m_BodyID = id.GetIndexAndSequenceNumber();

    m_Initialized = true;
}

void RigidBodyComponent::Update()
{
    if (!m_Initialized || !m_PhysicsEngine) return;

    JPH::BodyID realID(m_BodyID);
    glm::mat4 physicsMatrix = m_PhysicsEngine->GetMatrix(realID);

    m_Owner->transform->ModelMatrix = physicsMatrix * glm::scale(glm::mat4(1.0f), m_Owner->transform->Scale);
}