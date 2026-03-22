#pragma once

#include <glm/glm.hpp>

class Quat
{
public:
    float w;
    float x;
    float y;
    float z;
    Quat();
    Quat(float w, float x, float y, float z);
    Quat(glm::vec3 vec, float w = 0);
    Quat conjugate();
    float length();
    Quat normalize();
    glm::vec3 rotate_quat(glm::vec3 point, glm::vec3 axis, float angle);
    friend Quat operator*(const Quat& quat1, const Quat& quat2)
    {
        float w = (quat1.w * quat2.w) - (quat1.x * quat2.x) - (quat1.y * quat2.y) - (quat1.z * quat2.z);
        float x = (quat1.w * quat2.x) + (quat1.x * quat2.w) + (quat1.y * quat2.z) - (quat1.z * quat2.y);
        float y = (quat1.w * quat2.y) - (quat1.x * quat2.z) + (quat1.y * quat2.w) + (quat1.z * quat2.x);
        float z = (quat1.w * quat2.z) + (quat1.x * quat2.y) - (quat1.y * quat2.x) + (quat1.z * quat2.w);
        return Quat(w, x, y, z);
    }
};