#include <glm/glm.hpp>
#include "GameObject.h"
#include "Engine/InputManager.h"
#include "Game/Scripts/SpawnManager.h"

class GameObject;

class Player : public Behaviour
{
private:
    InputName m_InputName;

    void HandleActionPressed();
    void HandleThrowPressed();
    void HandleThrowReleased();
public:
    glm::vec2 moveInput{ 0.0f };
    float speed = 10.0f;
    int deviceID;

    Player(int deviceid);
    void Awake() override;
    void Update() override;

    GameObject* m_CarriedAnimal = nullptr;

    bool m_IsChargingThrow = false;
    float m_ThrowCharge = 0.0f;
    const float m_MinThrowForce = 5.0f;
    const float m_MaxThrowForce = 15.0f;
    const float m_ChargeSpeed = 10.0f;
};
