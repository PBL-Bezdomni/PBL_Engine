#include <Game/Scripts/Player.h>
#include "Engine/Engine.h"
#include <glm/gtc/quaternion.hpp>
#include <Game/Scripts/Animal.h>
#include <string>
#include <vector>
#include "Engine/InputManager.h"
#include "Engine/Components/RigidBody.h"
#include "Engine/Time.h"

Player::Player(int deviceid)
{
	deviceID = deviceid;
}

void Player::Awake()
{
    Behaviour::Awake();

    m_Owner->Name = "Player" + std::to_string(deviceID);
    
    Engine& engine = Engine::GetInstance();
    AssetManager* am = &engine.GetAssetManager();
    InputManager* im = &engine.GetGameManager().GetInputManager();
    string path = "res/models/players/";
    if (deviceID == 0)
    {
        path += "druid1/druid1.fbx";
    }
    else
    {
        path += "druid2/druid2.fbx";
    }
    Model bodyModel = *am->GetModel(*am->BasicShader, path.c_str());
    m_Owner->AddComponent<Model>(bodyModel);

    im->subscribe(deviceID, m_InputName.MOVE_FORWARD, [this](float val, InputEventType type, int id) {
        if(id == deviceID)
            this->moveInput.y = val;
        });

    im->subscribe(deviceID, m_InputName.MOVE_STRAFE, [this](float val, InputEventType type, int id) {
        if (id == deviceID)
            this->moveInput.x = val;
        });

    im->subscribe(deviceID, m_InputName.ACTION, [this](float val, InputEventType type, int id)
    {
        if (id == deviceID && type == InputEventType::Started)
            this->HandleActionPressed();
    });

    im->subscribe(deviceID, m_InputName.THROW, [this](float val, InputEventType type, int id)
    {
        if (id == deviceID && type == InputEventType::Started)
        {
            this->HandleThrowPressed();
        }
    });

    im->subscribe(deviceID, m_InputName.THROW, [this](float val, InputEventType type, int id)
    {
        if (id == deviceID && type == InputEventType::Ended)
        {
            this->HandleThrowReleased();
        }
    });
}


void Player::Update()
{
    float deltaTime = Time::GetDeltaTime();
	if (!m_Owner) return;

    RigidBody* rb = m_Owner->GetComponent<RigidBody>();

	glm::vec3 direction = glm::vec3(moveInput.x, 0.0f, moveInput.y);

    if (rb != nullptr)
    {
        if (glm::length(direction) > 1.0f) {
            direction = glm::normalize(direction);

            float targetAngle = glm::degrees(atan2(direction.x, direction.z));

            rb->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));

            if (glm::length(direction) > 0.01f)
            {
                // Save last input that moved player
                m_LastMoveDir = moveInput;
            }
        }

        glm::vec3 currentVel = rb->GetLinearVelocity();
        glm::vec3 targetVel = direction * speed;

        rb->SetLinearVelocity(glm::vec3(targetVel.x, currentVel.y, targetVel.z));
    }
    else
    {
        if (glm::length(direction) > 0.01f) {
            m_Owner->transform->Position += direction * speed * deltaTime;
        }
    }

    if (m_IsChargingThrow)
    {
        m_ThrowCharge += m_ChargeSpeed * deltaTime;
        if (m_ThrowCharge > m_MaxThrowForce) {
            m_ThrowCharge = m_MaxThrowForce;
        }
    }
    
    if (m_CarriedAnimal != nullptr)
    {

        glm::vec3 headPos = m_Owner->GetWorldPosition() + glm::vec3(0.0f, 3.0f, 0.0f);
        // m_CarriedAnimal->transform->Position = headPos;
        // m_CarriedAnimal->UpdateSelfAndChild();

        RigidBody* rb = m_CarriedAnimal->GetComponent<RigidBody>();
        Animal* a = m_CarriedAnimal->GetComponent<Animal>();
        if (rb != nullptr)
        {
            rb->SetLinearVelocity(glm::vec3(0.0f));
            rb->SetAngularVelocity(glm::vec3(0.0f));
            a->m_IsMoving = false;
            rb->Teleport(headPos);
        }
    }
}

void Player::HandleActionPressed()
{
    if (m_CarriedAnimal == nullptr)
    {   
        glm::vec3 playerPos = m_Owner->GetWorldPosition();

        playerPos.y += 0.5f;

        glm::vec3 rot = glm::vec3(m_LastMoveDir.x, 0, m_LastMoveDir.y);
        glm::vec3 playerForward = glm::normalize(rot);
        // glm::vec3 playerForward = glm::quat(glm::radians(m_Owner->transform->EulerAngles)) * glm::vec3(0.0f, 0.0f, 1.0f);

        float rayDistance = 5.0f;

        glm::vec3 endPos = playerPos + (playerForward * rayDistance);

        GameObject* hitObject = Engine::GetInstance().GetPhysicsEngine().CastRay(playerPos, playerForward, rayDistance, m_Owner->GetComponent<RigidBody>()->GetBodyID());

        if (hitObject != nullptr)
        {
            //spdlog::info("Check: {} {} {}", hitObject->transform->Position.x, hitObject->transform->Position.y, hitObject->transform->Position.z);

            Animal* animalScript = hitObject->GetDerivedComponent<Animal>();
            if (animalScript != nullptr)
            {

                m_CarriedAnimal = hitObject;

                animalScript->m_IsSeated = true;
            }
        }
    }
}

void Player::HandleThrowPressed()
{
    if (m_CarriedAnimal != nullptr)
    {
        m_IsChargingThrow = true;
    }
}

void Player::HandleThrowReleased()
{
    if (m_CarriedAnimal != nullptr && m_IsChargingThrow)
    {
        RigidBody* animalRb = m_CarriedAnimal->GetComponent<RigidBody>();
        Animal* animalScript = m_CarriedAnimal->GetDerivedComponent<Animal>();

        if (animalRb != nullptr)
        {
            glm::vec3 rot = glm::vec3(m_LastMoveDir.x, 0, m_LastMoveDir.y);
            glm::vec3 playerForward = glm::normalize(rot);
            // glm::vec3 playerForward = glm::quat(glm::radians(m_Owner->transform->EulerAngles)) * glm::vec3(0.0f, 0.0f, 1.0f);

            glm::vec3 throwVelocity = playerForward * m_ThrowCharge;
            throwVelocity.y = m_ThrowCharge * 0.4f;

            animalRb->SetLinearVelocity(throwVelocity);
        }

        if (animalScript != nullptr)
        {
            animalScript->m_IsSeated = false;
            animalScript->m_IsMoving = false;
			animalScript->m_WasDroppedByPlayer = true;
        }

        m_CarriedAnimal = nullptr;
        m_IsChargingThrow = false;
        m_ThrowCharge = m_MinThrowForce;
    }
}

