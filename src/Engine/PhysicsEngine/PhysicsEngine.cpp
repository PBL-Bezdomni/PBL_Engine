#include "PhysicsEngine.h"
#include "PhysicsMath.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "PhysicsDebugRenderer.h"

void PhysicsEngine::Init() {
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    m_TempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    m_JobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    m_PhysicsSystem = new JPH::PhysicsSystem();
    m_PhysicsSystem->Init(1024, 0, 1024, 1024, m_BPLayerInterface, m_ObjVsBPFilter, m_ObjVsObjFilter);

    //m_PhysicsSystem->SetContactListener(&m_ContactListener);

    m_DebugRenderer = new PhysicsDebugRenderer();
}

void PhysicsEngine::Update(float deltaTime) {
    if (m_PhysicsSystem) {
        m_PhysicsSystem->Update(deltaTime, 1, m_TempAllocator, m_JobSystem);
    }
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
    drawSettings.mDrawBoundingBox = false;
    drawSettings.mDrawShape = true;
    drawSettings.mDrawShapeWireframe = true;

    m_PhysicsSystem->DrawBodies(drawSettings, m_DebugRenderer, nullptr);
}