#include "Quaternion.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

Quaternion::Quaternion()
{
    this->w = 0;
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

Quaternion::Quaternion(glm::vec3 vec, float w)
{
    this->w = w;
    this->x = vec.x;
    this->y = vec.y;
    this->z = vec.z;
}

Quaternion Quaternion::Conjugate()
{
    return Quaternion(this->w, - this->x, - this->y, - this->z);
}

float Quaternion::Length()
{
    float w = this->w * this->w;
    float x = this->x * this->x;
    float y = this->y * this->y;
    float z = this->z * this->z;
    return sqrt(w + x + y + z);
}

Quaternion Quaternion::Normalize()
{
    float n = this->Length();
    float w = this->w / n;
    float x = this->x / n;
    float y = this->y / n;
    float z = this->z / n;
    return Quaternion(w, x, y, z);
}

glm::vec3 Quaternion::RotateQuaternion(glm::vec3 point, glm::vec3 axis, float angle)
{
    int angle_degrees = (int)angle % 360;
    angle_degrees /= 2;
    float rad_angle = (angle_degrees * M_PI) / 180;
    w = cos(rad_angle);
    axis = glm::normalize(axis);
    axis *= sin(rad_angle);
    Quaternion q = Quaternion(axis, w);
    Quaternion q_con = q.Conjugate();
    Quaternion p = Quaternion(point, 0.f);
    Quaternion p_prime = q * p * q_con;
    return glm::vec3(p_prime.x, p_prime.y, p_prime.z);
}