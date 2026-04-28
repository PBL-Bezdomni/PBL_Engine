#pragma once
#include <Jolt/Jolt.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace PhysicsMath {
    inline JPH::Vec3 ToJPH(const glm::vec3& v) {
        return JPH::Vec3(v.x, v.y, v.z);
    }

    inline glm::vec3 ToGLM(const JPH::Vec3& v) {
        return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
    }

    inline glm::mat4 ToGLM(const JPH::Mat44& mat) {
        glm::mat4 result;
        mat.StoreFloat4x4((JPH::Float4*)glm::value_ptr(result));
        return result;
    }
}