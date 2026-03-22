#pragma once
#include "Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glad/glad.h>

const float DEFAULT_X = 0.f;
const float DEFAULT_Y = 20.f;
const float DEFAULT_Z = 0.f;

const float SPEED_CHANGE = 2.f;
const float MAX_SPEED = 40.f;
const float MIN_SPEED = 2.f;
const float MAX_BODY_ROLL = 30.f;
const float MAX_BODY_PITCH = 60.f;
const float MAX_PITCH_INCREASE = 10.f;
const float MAX_YAW_INCREASE = 20.f;
const float DEFAULT_YAW = -90.f;
const float DEFAULT_PITCH_V = 0.f;
const float DEFAULT_ROLL = 0.f;
const float YAW_INCREASE_VALUE = 5.f;
const float PITCH_INCREASE_VALUE = 5.f;

class Airplane : public Entity
{
public:
	float Speed;
	float maxFlapTilt = 0.03f;
	float PropellerSpeedMult = 100.f;
	Airplane();
	Airplane(Shader shader);
	void SetDefault(bool setPosition = true);
	void Update(float deltaTime);
	glm::vec3 Front;
	float Yaw, Pitch, Roll;
	void UpdateYaw(int offset);
	void UpdatePitch(int offset);
	void UpdateSpeed(int offset);
	void SetRemoteControl(bool set);
	float YawIncrease;
	int PitchOffset = false;
	int YawOffset = false;
private:
	void UpdateAirplaneVectors();
	
	float PitchIncrease;
	bool IsRemotelyControlled = false;
	float SpeedOffset;
	glm::vec3 Up, Right;
	glm::vec3 WorldUp;
};
