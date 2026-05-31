#pragma once
#include "Engine/Components/Behaviour.h"
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include <memory>

#include "Engine/AssetManager.h"
#include "Game/SceneManager.h"

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
	AssetManager* m_AssetMgr;
	SceneManager* m_SceneMgr;

	float m_TimeLimit;
	float m_CurrTime;
    int m_minNeeds = 1;
    int m_maxNeeds = 4;

    int m_numberOfNeeds = 1;

    float m_SatisfactionSpeed = 0.1f;
    bool m_IsFulfillingNeed = false;
    AnimalNeeds m_CurrentNeedBeingFulfilled;
    float m_CurrentNeedProgress = 0.0f;

    bool m_ShouldTeleport = false;
    glm::vec3 m_TeleportTarget;

	// Walking parameters
	float m_MovingRadius = 10.0f;
    float m_MoveSpeed = 3.0f;
	float m_RotationSpeed = 2.0f;
	float m_CurrentAngle = 0.0f;
	float m_Acceleration = 2.0f;
	float m_JumpForce = 4.0f;
	float m_JumpTimer = 0.0f;

	glm::vec3 m_TargetPosition;
    glm::vec3 m_LastPosition;

	bool m_IsInitialized = false;
	float m_WaitTime = 0.0f;
	float m_CurrentWaitTime = 0.0f;
	float m_StuckTimer = 0.0f;

	void PickNewTargetPosition();

    std::shared_ptr<Shader> m_PieShader;
    std::shared_ptr<GameObject> m_Indicator;

    std::shared_ptr<Shader> m_ProgressBarShader;
    std::shared_ptr<GameObject> m_ProgressBar;
    void UpdateProgressBar();

public:
    std::vector<AnimalNeeds> m_RequiredServices;

    bool m_IsSeated = false;
    bool m_IsMoving = false;
	int Earned_money = 0;
    

	void Awake() override;
    void Start() override;
	void DrawUpdate() override;

    void EnterTable(GameObject* table);                // probably will
    void EnterPosition(glm::vec3 exactWorldPosition); // merge them

    void Update() override;

    void ForceNewTargetPosition();

    void SetProgressBarShader(std::shared_ptr<Shader> barShader);
    void SetIndicatorShader(std::shared_ptr<Shader> pieShader);
    void UpdateIndicatorColors();
    void SetIndicatorObject(std::shared_ptr<GameObject> indicator) { m_Indicator = indicator; }
    std::shared_ptr<GameObject> GetIndicatorObject() { return m_Indicator; }
	
	void DrawRandomNeeds();
    void StartFulfillingNeed(AnimalNeeds need);
    void StopFulfillingNeed();
    void FulfillNeed(AnimalNeeds need);

	const char* GetScriptName() const override { return "Animal"; }
};