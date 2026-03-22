#include "Airplane.h"

Airplane::Airplane()
{
    Yaw = DEFAULT_YAW;
    Pitch = DEFAULT_PITCH_V;
    Roll = DEFAULT_ROLL;
}

Airplane::Airplane(Shader shader) : Entity(shader)
{   
    Yaw = DEFAULT_YAW;
    Pitch = DEFAULT_PITCH_V;
    Roll = DEFAULT_ROLL;
}

void Airplane::SetDefault(bool setPosition)
{
    Speed = MAX_SPEED / 2;
    
    YawIncrease = MAX_YAW_INCREASE;
    PitchIncrease = 0;
    if (setPosition)
    {
        this->transform.pos = glm::vec3(DEFAULT_X, DEFAULT_Y, DEFAULT_Z);
    }
    WorldUp = glm::vec3(0.f, 1.f, 0.f);
    UpdateAirplaneVectors();
}

void Airplane::SetRemoteControl(bool set)
{
    IsRemotelyControlled = set;
    if (!IsRemotelyControlled)
    {
        SetDefault(false);
    }
    else
    {
        YawIncrease = 0;
        PitchIncrease = 0;
        Roll = 0;
    }
}

void Airplane::Update(float deltaTime)
{
    Speed += SpeedOffset * SPEED_CHANGE * deltaTime;
    if (Speed > MAX_SPEED)
    {
        Speed = MAX_SPEED;
    }
    else if (Speed < MIN_SPEED)
    {
        Speed = MIN_SPEED;
    }

    YawIncrease += YawOffset * YAW_INCREASE_VALUE * deltaTime;
    if (YawIncrease > MAX_YAW_INCREASE)
    {
        YawIncrease = MAX_YAW_INCREASE;
    }
    else if (YawIncrease < -MAX_YAW_INCREASE)
    {
        YawIncrease = -MAX_YAW_INCREASE;
    }

    Yaw += YawIncrease * deltaTime;
    Roll += YawIncrease * 2 * deltaTime;
    if (Roll > MAX_BODY_ROLL)
    {
        Roll = MAX_BODY_ROLL;
    }
    else if (Roll < -MAX_BODY_ROLL)
    {
        Roll = -MAX_BODY_ROLL;
    }
    Pitch += PitchOffset * PITCH_INCREASE_VALUE * deltaTime;
    if (Pitch > MAX_BODY_PITCH)
    {
        Pitch = MAX_BODY_PITCH;
    }
    else if (Pitch < -MAX_BODY_PITCH)
    {
        Pitch = -MAX_BODY_PITCH;
    }

    if (IsRemotelyControlled)
    {
        int sign;
        if (YawOffset == 0)
        {
            sign = YawIncrease > 0 ? -1 : 1;
            YawIncrease += sign * YAW_INCREASE_VALUE * deltaTime;
            
            if ((YawIncrease < 0 && sign < 0) || (YawIncrease > 0 && sign > 0))
            {
                YawIncrease = 0;
            }

            sign = Roll > 0 ? -1 : 1;
            Roll += sign * YAW_INCREASE_VALUE * 4 * deltaTime;
            if ((Roll < 0 && sign < 0) || (Roll > 0 && sign > 0))
            {
                Roll = 0;
            }
        }
        
        if (PitchOffset == 0)
        {
            sign = Pitch > 0 ? -1 : 1;
            Pitch += sign * PITCH_INCREASE_VALUE * deltaTime;
            if ((Pitch < 0 && sign < 0) || (Pitch > 0 && sign > 0))
            {
                Pitch = 0;
            }
            if ((Pitch < 0 && sign < 0) || (Pitch > 0 && sign > 0))
            {
                Pitch = 0;
            }
        }
    }

    UpdateAirplaneVectors();
}

void Airplane::UpdatePitch(int offset)
{
    PitchOffset = offset;
}

void Airplane::UpdateYaw(int offset)
{
    YawOffset = offset;
}

void Airplane::UpdateSpeed(int offset)
{
    SpeedOffset = offset;
}

void Airplane::UpdateAirplaneVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
