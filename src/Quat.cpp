#include "Quat.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

Quat::Quat()
{
    this->w = 0;
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Quat::Quat(float w, float x, float y, float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

Quat::Quat(glm::vec3 vec, float w)
{
    this->w = w;
    this->x = vec.x;
    this->y = vec.y;
    this->z = vec.z;
}

Quat Quat::conjugate()
{
    return Quat(this->w, - this->x, - this->y, - this->z);
}

float Quat::length()
{
    float w = this->w * this->w;
    float x = this->x * this->x;
    float y = this->y * this->y;
    float z = this->z * this->z;
    return sqrt(w + x + y + z);
}

Quat Quat::normalize()
{
    float n = this->length();
    float w = this->w / n;
    float x = this->x / n;
    float y = this->y / n;
    float z = this->z / n;
    return Quat(w, x, y, z);
}

glm::vec3 Quat::rotate_quat(glm::vec3 point, glm::vec3 axis, float angle)
{
    int angle_degrees = (int)angle % 360;
    angle_degrees /= 2;
    float rad_angle = (angle_degrees * M_PI) / 180;
    w = cos(rad_angle);
    axis = glm::normalize(axis);
    axis *= sin(rad_angle);
    Quat q = Quat(axis, w);
    Quat q_con = q.conjugate();
    Quat p = Quat(point, 0.f);
    Quat p_prime = q * p * q_con;
    return glm::vec3(p_prime.x, p_prime.y, p_prime.z);
}