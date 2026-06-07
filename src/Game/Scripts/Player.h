#include <glm/glm.hpp>
#include "GameObject.h"
#include "Engine/InputManager.h"
#include "Game/Scripts/SpawnManager.h"

class ParticleEmitter;
class GameObject;

class Player : public Behaviour
{
private:
    InputName m_InputName;
    glm::vec2 m_LastMoveDir = glm::vec2(0);

    ParticleEmitter* m_ParticleEmitter;

    void BindInput();
    
    void HandleActionPressed();
    void HandleThrowPressed();
    void HandleThrowReleased();

    shared_ptr<GameObject> m_ChargeMeter;
    shared_ptr<Shader> m_ChargeMeterShader;
public:
    glm::vec2 moveInput{ 0.0f };
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
};
