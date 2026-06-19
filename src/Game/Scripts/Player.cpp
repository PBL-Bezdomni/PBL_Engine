#include <Game/Scripts/Player.h>
#include "Engine/Engine.h"
#include <glm/gtc/quaternion.hpp>
#include <Game/Scripts/Animal.h>
#include <string>
#include <vector>

#include "TutorialArrow.h"
#include "Engine/InputManager.h"
#include "Engine/Components/RigidBody.h"
#include "Engine/Time.h"
#include "Engine/Components/ParticleEmitter.h"
#include "Engine/Components/TargetingZone.h"
#include "Game/Scripts/AOnsenObject.h"
#include <filesystem>
#include "Engine/Loader.h"
#include <cstdlib>
#include <ctime>

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
    m_SceneMgr = &engine.GetGameManager().GetSceneManager();
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

    m_ParticleEmitter = m_Owner->AddComponent<ParticleEmitter>();
    m_ParticleEmitter->Initialize("res/shaders/smokeParticles.vert", "res/shaders/smokeParticles.frag", "res/shaders/smokeParticles.comp", "res/models/PieChartPlane.obj", "res/textures/UI/smoke.png");

    m_ChargeMeterShader = am->GetShader("res/shaders/powerMeter.vert", "res/shaders/ProgressBar.frag");
    m_ChargeMeter = m_SceneMgr->Instantiate(m_Owner, "res/models/CheckmarkPlane.obj", m_ChargeMeterShader);
    m_ChargeMeter->GetComponent<Model>()->ReassignShader(*m_ChargeMeterShader);
    // Maybe custom texture for meter
    // m_ChargeMeter->GetComponent<Model>()->AssignTexture();
    m_ChargeMeter->transform->Position = glm::vec3(0.f, -0.7f, 4.f);
    m_ChargeMeter->transform->EulerAngles = glm::vec3(0.f, -90.f, 0.f);
    m_ChargeMeter->transform->Scale = glm::vec3(2.6f, 1.0f, 0.5f);
    m_ChargeMeter->SetActive(false);

    BindInput();

    m_OnsenObjects = m_SceneMgr->GetLevelParent()->FindComponentsInChildren<AOnsenObject>();

    // Initialize footstep audio clips from folder
    try {
        std::srand((unsigned)std::time(nullptr));
        std::string relDir = "res/audio/footsteps/Forest_ground";
        std::string fullDir = Loader::RelativePath() + relDir;
        if (std::filesystem::exists(fullDir))
        {
            for (auto& p : std::filesystem::directory_iterator(fullDir))
            {
                if (p.is_regular_file())
                {
                    std::string name = p.path().filename().string();
                    m_FootstepClips.push_back(relDir + "/" + name);
                }
            }
        }
    }
    catch (...) {
        // ignore filesystem errors
    }
}


void Player::Update()
{
    float deltaTime = Time::GetDeltaTime();
	if (!m_Owner) return;

    if (m_TargetingZones.empty()) {
        m_TargetingZones = m_Owner->FindComponentsInChildren<TargetingZone>();
        for (TargetingZone* zone : m_TargetingZones) {
            zone->LinkedPlayer = this;
        }
    }

    if (m_IgnoreThrownAnimalTimer > 0.0f)
    {
        m_IgnoreThrownAnimalTimer -= deltaTime;
        if (m_IgnoreThrownAnimalTimer <= 0.0f)
        {
            m_LastThrownAnimal = nullptr;
            if (m_CarriedAnimal == nullptr) {
                RecalculateBestTarget();
            }
        }
    }

    if (m_BestAnimalTarget != nullptr)
    {
        Animal* bestAnimalScript = m_BestAnimalTarget->GetDerivedComponent<Animal>();

        if (bestAnimalScript != nullptr && bestAnimalScript->m_StateController.GetCurrentState() == AnimalState::PickedUp)
        {
            SetHighlight(m_BestAnimalTarget, false);
            m_BestAnimalTarget = nullptr;
            m_BestAnimalScore = -1.0f;
            RecalculateBestTarget();
        }
        else if (m_CarriedAnimal == nullptr)
        {
            SetHighlight(m_BestAnimalTarget, true);
        }
    }

    RigidBody* rb = m_Owner->GetComponent<RigidBody>();

	glm::vec3 direction = glm::vec3(MoveInput.x, 0.0f, MoveInput.y);
    glm::vec3 lookAt = glm::vec3(LookInput.x, 0.f, LookInput.y);
    bool isLookingAwayFromMove = glm::length(lookAt) > 0.5f;

    if (glm::length(MoveInput) > 0.01f)
    {
        m_ParticleEmitter->Play();
        // Footstep handling
        if (!m_FootstepClips.empty())
        {
            m_FootstepTimer -= deltaTime;
            if (m_FootstepTimer <= 0.0f)
            {
                int idx = std::rand() % m_FootstepClips.size();
                Engine::GetInstance().GetAudioManager().PlaySound(m_FootstepClips[idx]);
                m_FootstepTimer = m_FootstepInterval;
            }
        }
    }
    else
    {
        m_ParticleEmitter->Stop();
        m_FootstepTimer = 0.0f;
    }

    if (rb != nullptr)
    {
        bool changeRotation = false;
        if (glm::length(direction) > 0.01f || glm::length(lookAt) > 0.01f)
        {
            changeRotation = true;
        }
        if (glm::length(direction) > 1.0f) {
            direction = glm::normalize(direction);
        }
        if (glm::length(lookAt) > 1.f)
        {
            lookAt = glm::normalize(lookAt);
        }

        if (changeRotation)
        {
            glm::vec3 rotVector = direction;
            if (isLookingAwayFromMove)
            {
                rotVector = lookAt;
            }

            float targetAngle = glm::degrees(atan2(rotVector.x, rotVector.z));

            rb->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));


            if (glm::length(rotVector) > 0.05f)
            {
                // Save last input that moved player
                m_LastLookDir = glm::vec2(rotVector.x, rotVector.z);
                if (glm::isnan(rotVector.x) || glm::isnan(rotVector.z))
                {
                    std::cout << "WARNING: Last look is nan\n";
                }
            }
        }

        if (glm::length(direction) > 0.05f)
        {
            // Save last input that moved player
            m_LastMoveDir = glm::vec2(direction.x, direction.z);;
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
            rb->Teleport(headPos);
            a->m_StateController.RequestStateChange.Invoke(AnimalState::PickedUp);
        }
    }

    glm::vec3 playerWorldPos = m_Owner->GetWorldPosition();

    glm::vec3 lookVec = glm::vec3(m_LastLookDir.x, 0.0f, m_LastLookDir.y);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

    if (glm::length(lookVec) > 0.001f) {
        forward = glm::normalize(lookVec);
    }

    float playerAngle = atan2(forward.x, forward.z);
    glm::quat playerRot = glm::quat(glm::vec3(0.0f, playerAngle, 0.0f));

    for (TargetingZone* zone : m_TargetingZones)
    {
        GameObject* zoneGO = zone->GetOwner();
        RigidBody* zoneRB = zoneGO->GetComponent<RigidBody>();

        if (zoneRB != nullptr)
        {
            if (zoneGO->Name == "TargetZone_Large_Air" || zoneGO->Name == "TargetZone_Small_Proximity")
            {
                zoneGO->transform->Position = playerWorldPos;
                zoneRB->MoveKinematic(playerWorldPos, playerRot);
            }
            else if (zoneGO->Name == "TargetZone_Medium_Front")
            {
                glm::vec3 offsetPos = playerWorldPos + (forward * 2.0f);

                zoneGO->transform->Position = offsetPos;
                zoneRB->MoveKinematic(offsetPos, playerRot);
            }
            else if (zoneGO->Name == "TargetZone_Precision_Front")
            {
                glm::vec3 offsetPos = playerWorldPos + (forward * 3.0f);

                zoneGO->transform->Position = offsetPos;
                zoneRB->MoveKinematic(offsetPos, playerRot);
            }
        }
    }
}

void Player::DrawUpdate()
{
    Behaviour::DrawUpdate();
    if (m_IsChargingThrow)
    {
        m_ChargeMeterShader->Use();
        m_ChargeMeterShader->SetFloat("u_Progress", m_ThrowCharge / m_MaxThrowForce);
    }
}

void Player::BindInput()
{
    InputManager* im = &Engine::GetInstance().GetGameManager().GetInputManager();
    
    im->subscribe(deviceID, m_InputName.MOVE_FORWARD, [this](float val, InputEventType type, int id) {
        if(id == deviceID)
            this->MoveInput.y = val;
        });

    im->subscribe(deviceID, m_InputName.MOVE_STRAFE, [this](float val, InputEventType type, int id) {
        if (id == deviceID)
            this->MoveInput.x = val;
        });

    im->subscribe(deviceID, m_InputName.LOOK_FORWARD, [this](float val, InputEventType type, int id) {
        if(id == deviceID)
            this->LookInput.y = val;
        });

    im->subscribe(deviceID, m_InputName.LOOK_STRAFE, [this](float val, InputEventType type, int id) {
        if (id == deviceID)
            this->LookInput.x = val;
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

void Player::HandleActionPressed()
{
    if (m_CarriedAnimal == nullptr)
    {   
        glm::vec3 playerPos = m_Owner->GetWorldPosition();

        // playerPos.y = 0.5f;

        glm::vec3 rot = glm::vec3(m_LastMoveDir.x, 0, m_LastMoveDir.y);
        glm::vec3 playerForward = glm::normalize(rot);

        GameObject* hitObject = m_BestAnimalTarget;

        if (hitObject != nullptr)
        {
            //std::cout << "Check: " << hitObject->transform->Position.x << " " << hitObject->transform->Position.y << " " << hitObject->transform->Position.z << std::endl;

            Animal* animalScript = hitObject->GetDerivedComponent<Animal>();

            if (animalScript == nullptr)
            {
                AOnsenObject* onsenObject = hitObject->GetDerivedComponent<AOnsenObject>();
                if (onsenObject != nullptr)
                {
                    //glm::vec3 sightOrigin = playerPos + glm::vec3(0.0f, 1.5f, 0.0f);
                    animalScript = onsenObject->GetAnimalForPlayer(playerPos, playerForward);
                    if (animalScript != nullptr)
                    {
                        hitObject = animalScript->GetGameObject();
                    }
                }
            }
            if (animalScript != nullptr)
            {
                m_CarriedAnimal = hitObject;
                m_HasPickUpReleased = false;
                animalScript->m_StateController.RequestStateChange.Invoke(AnimalState::PickedUp);
                hitObject->GetComponent<Model>()->m_IsHighlighted = false;
                vector<AnimalNeeds> services = animalScript->GetRequiredServices();
                // Play pickup sound
                Engine::GetInstance().GetAudioManager().PlaySound("res/audio/2.wav");
                for (AOnsenObject* obj : m_OnsenObjects)
                {
                    bool isNeeded = false;
                    for (AnimalNeeds service : services)
                    {
                        if (service == obj->GetNeed())
                        {
                            isNeeded = true;
                            break;
                        }
                    }
                    if (isNeeded)
                    {
                        TutorialArrow* arrow = obj->GetTutorialArrow();
                        if (arrow != nullptr)
                        {
                            if (deviceID == 0) {
                                arrow->m_ArrowColor = glm::vec3(0.18f, 0.36f, 0.54f);
                            }
                            else if (deviceID == 1) {
                                arrow->m_ArrowColor = glm::vec3(0.25f, 0.42f, 0.31f);
                            }
                            else {
                                arrow->m_ArrowColor = glm::vec3(1.0f, 1.0f, 1.0f);
                            }
                            arrow->SetActive(true);
                        }
                    }
                }
            }
        }
    }
}

void Player::HandleThrowPressed()
{
    if (m_CarriedAnimal != nullptr)
    {
        if (!m_HasPickUpReleased)
        {
            return;
        }
        m_IsChargingThrow = true;
        m_ChargeMeter->SetActive(true);
    }
}

void Player::HandleThrowReleased()
{
    if (!m_HasPickUpReleased && m_CarriedAnimal != nullptr)
    {
        m_HasPickUpReleased = true;
        return;
    }
    if (m_CarriedAnimal != nullptr && m_IsChargingThrow)
    {
        Engine::GetInstance().GetAudioManager().PlaySound("res/audio/4.wav");
        RigidBody* animalRb = m_CarriedAnimal->GetComponent<RigidBody>();
        Animal* animalScript = m_CarriedAnimal->GetDerivedComponent<Animal>();

        if (animalRb != nullptr)
        {
            glm::vec3 rot = glm::vec3(m_LastLookDir.x, 0, m_LastLookDir.y);
            glm::vec3 playerForward = glm::normalize(rot);
            // glm::vec3 playerForward = glm::quat(glm::radians(m_Owner->transform->EulerAngles)) * glm::vec3(0.0f, 0.0f, 1.0f);

            glm::vec3 throwVelocity = playerForward * m_ThrowCharge;
            throwVelocity.y = m_ThrowCharge * 0.4f;
            if (glm::isnan(throwVelocity.x) || glm::isnan(throwVelocity.z))
            {
                std::cout << "WARNING: Throw Velocity was NaN\n";
                throwVelocity = glm::vec3(1.0, 0, 0);
            }
            animalRb->SetLinearVelocity(throwVelocity);
            animalScript->m_StateController.RequestStateChange.Invoke(AnimalState::Throw);
            m_LastThrownAnimal = animalScript->GetOwner();
            m_IgnoreThrownAnimalTimer = 1.5f;
        }

        if (animalScript != nullptr)
        {
			animalScript->m_WasDroppedByPlayer = true;

            animalScript->m_WaitTime = 2.0f;
            animalScript->m_CurrentWaitTime = 0.0f;
            animalScript->m_StuckTimer = 0.0f;
            
        }

        m_CarriedAnimal = nullptr;
        m_IsChargingThrow = false;
        m_ChargeMeter->SetActive(false);
        m_ThrowCharge = m_MinThrowForce;
        for (AOnsenObject* obj : m_OnsenObjects)
        {
            TutorialArrow* arrow = obj->GetTutorialArrow();
            if (arrow != nullptr)
            {
                arrow->SetActive(false);
            }
        }
    }
}

void Player::SetHighlight(GameObject* animal, bool isHighlighted)
{
    if (animal == nullptr) return;

    animal->GetComponent<Model>()->m_IsHighlighted = isHighlighted;
}

void Player::OnAnimalEnteredZone(GameObject* animal, float score)
{
    if (m_CarriedAnimal != nullptr) return;

    Animal* animalScript = animal->GetDerivedComponent<Animal>();
    if (animalScript != nullptr && animalScript->m_StateController.GetCurrentState() == AnimalState::PickedUp) {
        return;
    }

    if (score > m_BestAnimalScore && animal != m_LastThrownAnimal)
    {
        if (m_BestAnimalTarget != nullptr)
        {
            SetHighlight(m_BestAnimalTarget, false);
        }

        m_BestAnimalTarget = animal;
        m_BestAnimalScore = score;

        SetHighlight(m_BestAnimalTarget, true);
    }
}

void Player::OnAnimalExitedZone(GameObject* animal)
{

    if (animal == m_BestAnimalTarget)
    {
        SetHighlight(m_BestAnimalTarget, false);
        m_BestAnimalTarget = nullptr;
        m_BestAnimalScore = -1.0f;

        RecalculateBestTarget();
    }
}

void Player::RecalculateBestTarget()
{
    if (m_CarriedAnimal != nullptr) return;

    if (m_BestAnimalTarget != nullptr) {
        SetHighlight(m_BestAnimalTarget, false);
    }
    m_BestAnimalTarget = nullptr;
    m_BestAnimalScore = -1.0f;

    for (TargetingZone* zone : m_TargetingZones)
    {
        for (GameObject* animal : zone->AnimalsInZone)
        {
            Animal* animalScript = animal->GetDerivedComponent<Animal>();
            if (animalScript == nullptr) continue;

            float score = zone->GetAnimalScore(animal);
            OnAnimalEnteredZone(animal, score);
        }
    }
}