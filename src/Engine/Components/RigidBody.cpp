#include "Engine/Engine.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

RigidBody::RigidBody()
{
    m_HalfExtents = glm::vec3(1);
    m_IsStatic = false;
    m_IsTrigger = false;
}

void RigidBody::PrepareInit(const glm::vec3& halfExtents, bool isStatic, bool isTrigger)
{
    m_HalfExtents = halfExtents;
    m_IsStatic = isStatic;
    m_IsTrigger = isTrigger;
}

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
    if (!m_Initialized)
    {
        Init(m_HalfExtents, m_IsStatic, m_IsTrigger);
    }
    
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

void RigidBody::Teleport(const glm::vec3& newWorldPosition)
{
    if (!m_Initialized || !m_PhysicsEngine) return;

    JPH::BodyInterface& bodyInterface = m_PhysicsEngine->GetSystem()->GetBodyInterface();

    bodyInterface.SetPosition(
        JPH::BodyID(m_BodyID),
        JPH::Vec3(newWorldPosition.x, newWorldPosition.y, newWorldPosition.z),
        JPH::EActivation::Activate
    );
}

void RigidBody::RequestTeleport(const glm::vec3& newWorldPosition)
{
    if (!m_Initialized || !m_PhysicsEngine) return;
    
    m_PhysicsEngine->QueueTeleport(this, newWorldPosition);
}

void RigidBody::SetAngularVelocity(const glm::vec3& velocity)
{
    if (!m_Initialized || !m_PhysicsEngine) return;

    m_PhysicsEngine->GetSystem()->GetBodyInterface().SetAngularVelocity(
        JPH::BodyID(m_BodyID),
        JPH::Vec3(velocity.x, velocity.y, velocity.z)
    );
}

void RigidBody::SetRotation(const glm::vec3& eulerAnglesDegrees)
{
    if (!m_Initialized || !m_PhysicsEngine) return;

    glm::vec3 radians = glm::radians(eulerAnglesDegrees);

    glm::quat q = glm::quat(radians);

    JPH::Quat joltQuat(q.x, q.y, q.z, q.w);

    m_PhysicsEngine->GetSystem()->GetBodyInterface().SetRotation(
        JPH::BodyID(m_BodyID),
        joltQuat,
        JPH::EActivation::Activate
    );
}

glm::vec3 RigidBody::GetPosition()
{
    if (!m_Initialized || !m_PhysicsEngine) return glm::vec3(0.0f);

    JPH::Vec3 joltPos = m_PhysicsEngine->GetSystem()->GetBodyInterface().GetPosition(JPH::BodyID(m_BodyID));

    return glm::vec3(joltPos.GetX(), joltPos.GetY(), joltPos.GetZ());
}


void RigidBody::SetHitboxSize(const glm::vec3& newHalfExtents)
{
    if (!m_Initialized || !m_PhysicsEngine) return;

    m_HalfExtents = newHalfExtents;

    JPH::BoxShapeSettings shapeSettings(JPH::Vec3(newHalfExtents.x, newHalfExtents.y, newHalfExtents.z));
    JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();

    if (shapeResult.IsValid())
    {
        JPH::BodyInterface& bodyInterface = m_PhysicsEngine->GetSystem()->GetBodyInterface();

        bool updateMassProperties = !m_IsStatic && !m_IsTrigger;

        bodyInterface.SetShape(JPH::BodyID(m_BodyID), shapeResult.Get(), updateMassProperties, JPH::EActivation::Activate);
    }
}

glm::vec3 RigidBody::GetHitBoxSize()
{
    return m_HalfExtents;
}

bool RigidBody::GetIsStatic()
{
    return m_IsStatic;
}

bool RigidBody::GetIsTrigger()
{
    return m_IsTrigger;
}
