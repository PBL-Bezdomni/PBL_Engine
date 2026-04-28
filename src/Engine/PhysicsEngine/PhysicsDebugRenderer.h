#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <glm/glm.hpp>

class Shader;

class PhysicsDebugRenderer : public JPH::DebugRendererSimple
{
public:
    PhysicsDebugRenderer();
    virtual ~PhysicsDebugRenderer() override;

    void Setup(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;

    virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight) override {}
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, JPH::DebugRenderer::ECastShadow inCastShadow) override {}

private:
    Shader* m_Shader = nullptr;
    unsigned int VAO, VBO;
};