#include <Player.h>
#include "Engine/Engine.h"
#include <glm/gtc/quaternion.hpp>
#include <Engine/Components/Animal.h>

Player::Player(InputManager& input, SpawnManager& spawner, Shader& shader, int deviceid)
{
	deviceID = deviceid;

	body = make_unique<GameObject>();
	AssetManager* am = &Engine::GetInstance().GetAssetManager();
	Model bodyModel = *am->GetModel(shader, "res/models/players/druid1/druid1.fbx");
	body->AddComponent<Model>(bodyModel);

	input.subscribe(deviceID, "MoveForward", [this](float val, InputEventType type, int id) {
		if(id == deviceID)
			this->moveInput.y = val;
		});

	input.subscribe(deviceID, "MoveStrafe", [this](float val, InputEventType type, int id) {
		if (id == deviceID)
			this->moveInput.x = val;
		});
}

void Player::Update(float deltaTime)
{
	if (!body) return;

    RigidBody* rb = body->GetComponent<RigidBody>();

	glm::vec3 direction = glm::vec3(moveInput.x, 0.0f, moveInput.y);

    if (rb != nullptr)
    {
        if (glm::length(direction) > 1.0f) {
            direction = glm::normalize(direction);

            float targetAngle = glm::degrees(atan2(direction.x, direction.z));

            rb->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));
        }

        glm::vec3 currentVel = rb->GetLinearVelocity();
        glm::vec3 targetVel = direction * speed;

        rb->SetLinearVelocity(glm::vec3(targetVel.x, currentVel.y, targetVel.z));
    }
    else
    {
        if (glm::length(direction) > 0.01f) {
            body->transform->Position += direction * speed * deltaTime;
        }
    }

    if (deviceID == 0 && glfwGetKey(Engine::GetInstance().GetWindowManager().GetWindowPointer(), GLFW_KEY_F) == GLFW_PRESS)
    {

        if (m_CarriedAnimal == nullptr)
        {   
            glm::vec3 playerPos = body->transform->Position;

            //playerPos.y -= 2.0f;

            glm::vec3 playerForward = glm::quat(glm::radians(body->transform->EulerAngles)) * glm::vec3(0.0f, 0.0f, -1.0f);

            float rayDistance = 2.0f;

            glm::vec3 endPos = playerPos + (playerForward * rayDistance);
            
            GameObject* hitObject = Engine::GetInstance().GetPhysicsEngine().CastRay(playerPos, playerForward, rayDistance);
            //working on it
            Engine::GetInstance().GetPhysicsEngine().DrawDebugLine(playerPos, endPos, glm::vec3(1, 0, 0));
            Engine::GetInstance().GetPhysicsEngine().DrawDebugBox(playerPos, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (hitObject != nullptr)
            {
                Animal* animalScript = hitObject->GetDerivedComponent<Animal>();
                if (animalScript != nullptr)
                {

                    m_CarriedAnimal = hitObject;

                    animalScript->m_IsSeated = true;
                }
            }
        }
    }


    if (m_CarriedAnimal != nullptr)
    {

        glm::vec3 headPos = body->GetWorldPosition() + glm::vec3(0.0f, 3.0f, 0.0f);


        m_CarriedAnimal->transform->Position = headPos;
        m_CarriedAnimal->UpdateSelfAndChild();

        RigidBody* rb = m_CarriedAnimal->GetComponent<RigidBody>();
        if (rb != nullptr)
        {
            rb->SetLinearVelocity(glm::vec3(0.0f));
            rb->SetAngularVelocity(glm::vec3(0.0f));
            rb->Teleport(headPos);
        }
    }
}


