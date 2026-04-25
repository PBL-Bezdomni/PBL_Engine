#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include "PhysicsLayers.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Shader;
class PhysicsDebugRenderer;

class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine();

    void Init();
    void Update(float deltaTime);
    void Cleanup();

    JPH::BodyID CreateBox(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& halfExtents, bool isStatic);

    glm::mat4 GetMatrix(JPH::BodyID bodyID);
    void DrawHitboxes(Shader& lineShader, const glm::mat4& view, const glm::mat4& projection);

private:
    JPH::PhysicsSystem* m_PhysicsSystem = nullptr;
    JPH::TempAllocatorImpl* m_TempAllocator = nullptr;
    JPH::JobSystemThreadPool* m_JobSystem = nullptr;
    PhysicsDebugRenderer* m_DebugRenderer = nullptr;

    BPLayerInterfaceImpl m_BPLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl m_ObjVsBPFilter;
    ObjectLayerPairFilterImpl m_ObjVsObjFilter;
};