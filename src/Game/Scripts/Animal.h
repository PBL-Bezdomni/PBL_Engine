#pragma once
#include "Engine/Components/Behaviour.h"
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include <memory>

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
	float m_WaitTime = 0.0f;
	float m_CurrentWaitTime = 0.0f;
	float m_StuckTimer = 0.0f;

	void PickNewTargetPosition();

    std::shared_ptr<Shader> m_PieShader;
    std::shared_ptr<GameObject> m_Indicator;

public:
    std::vector<AnimalNeeds> m_RequiredServices;

    bool m_IsSeated = false;
    bool m_IsMoving = false;

	void Awake() override;
    void Start() override;

    void EnterTable(GameObject* table);                // probably will
    void EnterPosition(glm::vec3 exactWorldPosition); // merge them

    void Update() override;

    void SetIndicatorShader(std::shared_ptr<Shader> pieShader);
    void UpdateIndicatorColors();
    void SetIndicatorObject(std::shared_ptr<GameObject> indicator) { m_Indicator = indicator; }
    std::shared_ptr<GameObject> GetIndicatorObject() { return m_Indicator; }

    void FulfillNeed(AnimalNeeds need);
};