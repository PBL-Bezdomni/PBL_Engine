#pragma once

#include <glm/glm.hpp>

class Quaternion
{
public:
    float w;
    float x;
    float y;
    float z;
    Quaternion();
    Quaternion(float w, float x, float y, float z);
    Quaternion(glm::vec3 vec, float w = 0);
    Quaternion Conjugate();
    float Length();
    Quaternion Normalize();
    glm::vec3 RotateQuaternion(glm::vec3 point, glm::vec3 axis, float angle);
    friend Quaternion operator*(const Quaternion& quat1, const Quaternion& quat2)
    {
        float w = (quat1.w * quat2.w) - (quat1.x * quat2.x) - (quat1.y * quat2.y) - (quat1.z * quat2.z);
        float x = (quat1.w * quat2.x) + (quat1.x * quat2.w) + (quat1.y * quat2.z) - (quat1.z * quat2.y);
        float y = (quat1.w * quat2.y) - (quat1.x * quat2.z) + (quat1.y * quat2.w) + (quat1.z * quat2.x);
        float z = (quat1.w * quat2.z) + (quat1.x * quat2.y) - (quat1.y * quat2.x) + (quat1.z * quat2.w);
        return Quaternion(w, x, y, z);
    }
};