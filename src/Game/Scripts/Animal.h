#pragma once
#include "Engine/Components/Behaviour.h"
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include <memory>
#include "Engine/AssetManager.h"
#include "Game/SceneManager.h"
#include "AnimalInteractions.h"

class GameObject;
class AOnsenObject;

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
	AssetManager*       m_AssetMgr;
	SceneManager*       m_SceneMgr;
	AnimalInteractions m_AnimalInteractions;
	Camera* m_MainCamera;

	float m_TimeLimit;
	float m_CurrTime;
    int m_minNeeds = 1;
    int m_maxNeeds = 4;

    int m_numberOfNeeds = 1;

    float m_SatisfactionSpeed = 0.1f;
    bool m_IsFulfillingNeed = false;
    bool m_IsSeatedInObject = false;
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

	void PickNewTargetPosition();

    std::shared_ptr<Shader> m_PieShader;
    std::shared_ptr<GameObject> m_Indicator;

    std::shared_ptr<Shader> m_ProgressBarShader;
    std::shared_ptr<GameObject> m_ProgressBar;
    void UpdateProgressBar();

    std::shared_ptr<Shader> m_CheckmarkShader;
    std::shared_ptr<GameObject> m_Checkmark;

public:
    AOnsenObject* m_CurrentOnsen = nullptr;
    std::vector<AnimalNeeds> m_RequiredServices;
    bool m_IsInitialized = false;
    bool m_WasDroppedByPlayer = false;
    bool m_IsSeated = false;
    bool m_IsMoving = false;
    bool m_IsCarried = false;
	int Earned_money = 0;
    float m_WaitTime = 0.0f;
    float m_CurrentWaitTime = 0.0f;
    float m_StuckTimer = 0.0f;
    void ResetEverythingSpawn(glm::vec3 spawnPosition);

	void Awake() override;
    void Start() override;
	void DrawUpdate() override;

    void EnterTable(GameObject* table);                // probably will
    void EnterPosition(glm::vec3 exactWorldPosition); // merge them

    void Update() override;

    void ForceNewTargetPosition();

    void SetProgressBarShader(std::shared_ptr<Shader> barShader);
    void SetIndicatorShader(std::shared_ptr<Shader> pieShader);
    void SetCheckmarkShader(std::shared_ptr<Shader> checkmarkShader);
    void UpdateIndicatorColors();
    void SetIndicatorObject(std::shared_ptr<GameObject> indicator) { m_Indicator = indicator; }
    std::shared_ptr<GameObject> GetIndicatorObject() { return m_Indicator; }
    void SetTartgetPosition(glm::vec3 position) { m_TargetPosition = position; m_IsMoving = true; }
	void SetSpeed(float speed) { m_MoveSpeed = speed; }
	void DrawRandomNeeds();
    void StartFulfillingNeed(AnimalNeeds need);
    void StopFulfillingNeed();
    void FulfillNeed(AnimalNeeds need);

	GameObject* GetGameObject() { return m_Owner; }
    
	const char* GetScriptName() const override { return "Animal"; }
};