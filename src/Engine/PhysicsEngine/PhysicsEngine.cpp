#include "PhysicsEngine.h"
#include "PhysicsMath.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "PhysicsDebugRenderer.h"
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include "GameObject.h"
#include <Jolt/Physics/Body/BodyID.h>

void PhysicsEngine::Init() {
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    m_TempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    m_JobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    m_PhysicsSystem = new JPH::PhysicsSystem();
    m_PhysicsSystem->Init(1024, 0, 1024, 1024, m_BPLayerInterface, m_ObjVsBPFilter, m_ObjVsObjFilter);

    m_PhysicsSystem->SetContactListener(&m_ContactListener);

    m_DebugRenderer = new PhysicsDebugRenderer();
}

void PhysicsEngine::Update(float deltaTime) {
    if (m_PhysicsSystem) {
        m_PhysicsSystem->Update(deltaTime, 1, m_TempAllocator, m_JobSystem);
    }
    CompleteTeleportQueue();
}

JPH::BodyID PhysicsEngine::CreateBox(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& halfExtents, bool isStatic) {
    JPH::BoxShapeSettings boxSettings(PhysicsMath::ToJPH(halfExtents));
    JPH::ShapeRefC shape = boxSettings.Create().Get();

    JPH::EMotionType motionType = isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic;
    JPH::ObjectLayer layer = isStatic ? Layers::NON_MOVING : Layers::MOVING;

    JPH::Quat joltRot(rotation.x, rotation.y, rotation.z, rotation.w);

    JPH::BodyCreationSettings settings(shape, PhysicsMath::ToJPH(position), joltRot, motionType, layer);

    JPH::BodyInterface* bodyInterface = &m_PhysicsSystem->GetBodyInterface();
    JPH::Body* body = bodyInterface->CreateBody(settings);
    bodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);

    return body->GetID();
}

glm::mat4 PhysicsEngine::GetMatrix(JPH::BodyID bodyID) {
    JPH::BodyInterface* bodyInterface = &m_PhysicsSystem->GetBodyInterface();
    return PhysicsMath::ToGLM(bodyInterface->GetWorldTransform(bodyID));
}

PhysicsEngine::~PhysicsEngine()
{
    Cleanup();
}

void PhysicsEngine::Cleanup() {
    delete m_PhysicsSystem;
    delete m_JobSystem;
    delete m_TempAllocator;
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void PhysicsEngine::DrawHitboxes(Shader& lineShader, const glm::mat4& view, const glm::mat4& projection) {
    if (!m_PhysicsSystem || !m_DebugRenderer) return;

    m_DebugRenderer->Setup(lineShader, view, projection);

    JPH::BodyManager::DrawSettings drawSettings;
    drawSettings.mDrawBoundingBox = true;
    drawSettings.mDrawShape = true;
    drawSettings.mDrawShapeWireframe = true;

    m_PhysicsSystem->DrawBodies(drawSettings, m_DebugRenderer, nullptr);
}

GameObject* PhysicsEngine::CastRay(const glm::vec3& startOrigin, const glm::vec3& direction, float distance, uint32_t ignoreBodyID)
{
    if (!m_PhysicsSystem) return nullptr;

    JPH::RVec3 origin(startOrigin.x, startOrigin.y, startOrigin.z);
    JPH::Vec3 dir(direction.x * distance, direction.y * distance, direction.z * distance);

    JPH::RRayCast ray(origin, dir);
    JPH::RayCastResult result;

    JPH::IgnoreSingleBodyFilter bodyFilter{ JPH::BodyID(ignoreBodyID) };

    if (m_PhysicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, {}, {}, bodyFilter))
    {
        JPH::BodyID hitBodyID = result.mBodyID;

        JPH::BodyInterface& bodyInterface = m_PhysicsSystem->GetBodyInterface();

        if (bodyInterface.IsAdded(hitBodyID))
        {
            uint64_t userData = bodyInterface.GetUserData(hitBodyID);
            if (userData != 0)
            {
                return reinterpret_cast<GameObject*>(userData);
            }
        }
    }

    return nullptr;
}

void PhysicsEngine::DrawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
{
    if (m_PhysicsSystem == nullptr) return;

    JPH::RVec3 p1(start.x, start.y, start.z);
    JPH::RVec3 p2(end.x, end.y, end.z);
    JPH::Color joltColor(color.r, color.g, color.b);

    m_DebugRenderer->DrawLine(p1, p2, joltColor);
}

void PhysicsEngine::DrawDebugBox(const glm::vec3& center, const glm::vec3& halfExtents, const glm::vec3& color)
{
    if (m_DebugRenderer == nullptr) return;

    JPH::RVec3 minBound(center.x - halfExtents.x, center.y - halfExtents.y, center.z - halfExtents.z);
    JPH::RVec3 maxBound(center.x + halfExtents.x, center.y + halfExtents.y, center.z + halfExtents.z);

    JPH::AABox box(minBound, maxBound);

    JPH::Color joltColor(color.r * 255.0f, color.g * 255.0f, color.b * 255.0f);

    m_DebugRenderer->DrawWireBox(box, joltColor);
}

void PhysicsEngine::QueueTeleport(RigidBody* rb, glm::vec3 pos)
{
    if (rb != nullptr)
    {
        JPH::BodyID body = JPH::BodyID(rb->GetBodyID());
        JPH::RVec3 p(pos.x, pos.y, pos.z);

        m_TeleportQueue.push_back(TeleportRequest(body, p));
    }
}

void PhysicsEngine::CompleteTeleportQueue()
{
    for (TeleportRequest req : m_TeleportQueue)
    {
        JPH::BodyInterface& bodyInterface = m_PhysicsSystem->GetBodyInterface();
        bodyInterface.SetPosition(req.body, req.position, JPH::EActivation::Activate);
    }
    m_TeleportQueue.clear();
}


