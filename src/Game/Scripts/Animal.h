#pragma once
#include "Engine/Components/Behaviour.h"
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include <memory>
#include "Engine/AssetManager.h"
#include "Game/SceneManager.h"
#include "AnimalInteractions.h"
#include "AnimalStateController.h"
#include "Engine/Components/ParticleEmitter.h"

class RigidBody;
class GameObject;
class AOnsenObject;
class Player;

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
    std::vector<Player*> m_PlayersInScene;
	RigidBody* m_RB;

	float m_TimeLimit;
	float m_CurrTime;
    int m_minNeeds = 1;
    int m_maxNeeds = 4;

    int m_numberOfNeeds = 1;
    float m_CurrentNeedProgress = 0.0f;

    float m_SatisfactionSpeed = 0.1f;
    float m_PlayerFulfillSpeed = 0.2f;
    float m_SatisfactionDecreaseSpeed = 0.03f;
    AnimalNeeds m_CurrentNeedBeingFulfilled;

    bool m_ShouldTeleport = false;
    glm::vec3 m_TeleportTarget;

	// Walking parameters
	float m_MovingRadius = 10.0f;
    float m_MoveSpeed = 3.0f;
    float m_DefaultSpeed = 3.0f;
	float m_RotationSpeed = 2.0f;
	float m_CurrentAngle = 0.0f;
	float m_Acceleration = 2.0f;
	float m_JumpForce = 4.0f;
	float m_JumpTimer = 0.0f;

	glm::vec3 m_TargetPosition;
    glm::vec3 m_LastPosition;

	ParticleEmitter* m_WalkEmitter;
	ParticleEmitter* m_LandEmitter;
	ParticleEmitter* m_InteractionEmitter;

	void PickNewTargetPosition();

    std::shared_ptr<Shader> m_PieShader;
    std::shared_ptr<GameObject> m_Indicator;

    std::shared_ptr<Shader> m_ProgressBarShader;
    void UpdateProgressBar();

    std::shared_ptr<Shader> m_CheckmarkShader;
    std::shared_ptr<GameObject> m_Checkmark;
    std::shared_ptr<GameObject> m_InteractionMark;
    void UpdateCheckmark();

    std::shared_ptr<Shader> m_CellShadingShader;

	std::vector<AnimalNeeds> m_RequiredServices;
	// RigidBody* m_RB;

    AnimalStateController m_StateController;

    void AssignBearTexture();
	virtual void AssignWalkEmitter();
	virtual void AssignLandEmitter();
	virtual void AssignInteractionEmitter();

    std::shared_ptr<Shader> m_ObjectNeedsShader;
    std::vector<std::shared_ptr<GameObject>> m_NeedIcons;
    float m_IconYOffset = 40.0f;
    float m_IconSpacing = 5.0f;
    float m_IconScale = 1.7f;
    float m_ModelScaler = 1.0f;
    glm::vec3 m_ModelOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    void UpdateObjectIcons();
    void SetObjectIcons();

public:
	AEvent<AnimalNeeds> OnEnteredOnsenObject;
	
    std::shared_ptr<GameObject> m_ProgressBar;
    AOnsenObject* m_CurrentOnsen = nullptr;
    bool m_IsInitialized = false;
    bool m_WasDroppedByPlayer = false;
	int Earned_money = 0;
    float m_WaitTime = 0.0f;
    float m_CurrentWaitTime = 0.0f;
    float m_StuckTimer = 0.0f;
    bool m_IsHighlighted = true;
    void ResetEverythingSpawn(glm::vec3 spawnPosition);

	void Awake() override;
    void Start() override;
	void DrawUpdate() override;

    void EnterTable(GameObject* table);                
    void EnterPosition(glm::vec3 exactWorldPosition);

    void Update() override;
    void UpdateIdle();
    void UpdateWalking();
    void UpdateChasing();
    void UpdateEating();
    void UpdatePickedUp();
    void UpdateThrow();
    void UpdateFulfillingNeed();
    void UpdateCheckIn();

    void ForceNewTargetPosition();
	std::vector<AnimalNeeds> GetRequiredServices();

    void SetProgressBarShader(std::shared_ptr<Shader> barShader);
    void SetIndicatorShader(std::shared_ptr<Shader> pieShader);
    void SetCheckmarkShader(std::shared_ptr<Shader> checkmarkShader);
    void UpdateIndicatorColors();
    void SetIndicatorObject(std::shared_ptr<GameObject> indicator) { m_Indicator = indicator; }
    std::shared_ptr<GameObject> GetIndicatorObject() { return m_Indicator; }
    void SetTargetPosition(glm::vec3 position) { m_TargetPosition = position; }
	void SetSpeed(float speed) { m_MoveSpeed = speed; }
	void DrawRandomNeeds();
    void StartFulfillingNeed(AnimalNeeds need);
    void StopFulfillingNeed();
    void FulfillNeed(AnimalNeeds need);
    void PlayerFulfillNeed();
    bool IsCurrentNeedInteractible();

    Animal() : m_StateController(this) {}
    AnimalStateController* GetStateController() { return &m_StateController; }
    EventBinder m_EventBinder;
	GameObject* GetGameObject() { return m_Owner; }
    
	const char* GetScriptName() const override { return "Animal"; }
};