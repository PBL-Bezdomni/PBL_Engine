#pragma once
#include "Behaviour.h"
#include <glm/glm.hpp>
#include <vector>

class GameObject;

enum class AnimalNeeds
{
    Bath,
    Towels,
    Sauna,
    Massage
};

class Animal : public Behaviour
{
private:
    int m_minNeeds = 1;
    int m_maxNeeds = 4;

    int m_numberOfNeeds = 1;

    bool m_ShouldTeleport = false;
    glm::vec3 m_TeleportTarget;

	// Walking parameters
	float m_MovingRadius = 10.0f;
    float m_MoveSpeed = 3.0f;

	glm::vec3 m_TargetPosition;
    glm::vec3 m_LastPosition;

	bool m_IsInitialized = false;
	bool m_IsMoving = false;
	float m_WaitTime = 0.0f;
	float m_CurrentWaitTime = 0.0f;
	float m_StuckTimer = 0.0f;

	void PickNewTargetPosition();

public:
    std::vector<AnimalNeeds> m_RequiredServices;

    bool m_IsSeated = false;

    void Init();

    void EnterTable(GameObject* table);

    void Update() override;
};