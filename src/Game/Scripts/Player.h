#include <glm/glm.hpp>
#include "GameObject.h"
#include "Engine/InputManager.h"
#include "Game/Scripts/SpawnManager.h"

class AOnsenObject;
class ParticleEmitter;
class GameObject;

class Player : public Behaviour
{
private:
    SceneManager* m_SceneMgr;
    InputName m_InputName;
    glm::vec2 m_LastMoveDir = glm::vec2(0);
    glm::vec2 m_LastLookDir = glm::vec2(0);

    ParticleEmitter* m_ParticleEmitter;

    bool  m_HasPickUpReleased;

    void BindInput();
    
    void HandleActionPressed();
    void HandleThrowPressed();
    void HandleThrowReleased();

    shared_ptr<GameObject> m_ChargeMeter;
    shared_ptr<Shader> m_ChargeMeterShader;

    vector<AOnsenObject*> m_OnsenObjects;
    // Footstep audio
    std::vector<std::string> m_FootstepClips;
    float m_FootstepTimer = 0.0f;
    float m_FootstepInterval = 0.45f; // seconds between footsteps while moving

    std::vector<class TargetingZone*> m_TargetingZones;
public:
    glm::vec2 MoveInput{ 0.0f };
    glm::vec2 LookInput{ 0.0f };
    float speed = 10.0f;
    int deviceID;

    Player(int deviceid);
    void Awake() override;
    void Update() override;
    void DrawUpdate() override;

    GameObject* m_CarriedAnimal = nullptr;

    bool m_IsChargingThrow = false;
    float m_ThrowCharge = 0.0f;
    const float m_MinThrowForce = 5.0f;
    const float m_MaxThrowForce = 40.0f;
    const float m_ChargeSpeed = 10.0f;

    const char* GetScriptName() const override { return "Player"; }

    GameObject* m_BestAnimalTarget = nullptr;
    float m_BestAnimalScore = -1.0f;

    void OnAnimalEnteredZone(GameObject* animal, float score);
    void OnAnimalExitedZone(GameObject* animal);
    void RecalculateBestTarget();

    void SetHighlight(GameObject* animal, bool isHighlighted);
};
