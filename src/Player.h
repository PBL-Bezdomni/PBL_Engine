#include <glm/glm.hpp>
#include "Shader.h"
#include <string>
#include <vector>

#include "GameObject.h"
#include "Engine/InputManager.h"
#include "Game/Scripts/SpawnManager.h"
#include "Engine/Components/RigidBody.h"

class GameObject;

class Player {
public:
    unique_ptr<GameObject> body = nullptr;
    glm::vec2 moveInput{ 0.0f };
    float speed = 10.0f;
    int deviceID;

    Player(InputManager& input, SpawnManager& spawner, Shader& shader, int deviceid);
    void Update(float deltaTime);

    GameObject* m_CarriedAnimal = nullptr;

    bool m_IsChargingThrow = false;
    float m_ThrowCharge = 0.0f;
    const float m_MinThrowForce = 5.0f;
    const float m_MaxThrowForce = 15.0f;
    const float m_ChargeSpeed = 10.0f;
};
