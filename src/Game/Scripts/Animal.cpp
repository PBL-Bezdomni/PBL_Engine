#include "Animal.h"
#include "GameObject.h"
#include "Engine/Components/RigidBody.h"
#include <random>
#include <Random.h>
#include <Engine/Time.h>
#include <Engine/Engine.h>
#include "Engine/AssetManager.h"
#include <Engine/Animation/Animator.h>

#include "InteractionGlyph.h"
#include "Player.h"
#include "Engine/Components/ParticleEmitter.h"

void Animal::Awake()
{
    Behaviour::Awake();
    
    m_AssetMgr = &Engine::GetInstance().GetAssetManager();
    m_SceneMgr = &Engine::GetInstance().GetGameManager().GetSceneManager();
	m_TimeLimit = m_SceneMgr->GetTimeLimit();
	m_CurrTime = m_SceneMgr->GetTimeLeft();
	m_MainCamera = m_SceneMgr->GetMainCamera().get();

    m_RB = m_Owner->AddComponent<RigidBody>();
	m_Indicator = m_SceneMgr->Instantiate(m_Owner, "res/models/primitives/plane.obj", m_AssetMgr->PieChartShader);
	m_Indicator->Name = "NeedsIndicator";
	m_Indicator->transform->Position = glm::vec3(0.f, -0.8f, 0.f);
    AssignWalkEmitter();
    AssignInteractionEmitter();
    // TODO add scripts for every animal kind, that will override enum
    if (m_Owner->Name.find("bunny") != std::string::npos)
    {
        m_Indicator->transform->Position = glm::vec3(0.f, -3.4f, 0.f);
        m_Indicator->transform->Scale = glm::vec3(8.0f);
        if (m_RB != nullptr)
        {
            m_RB->PrepareInit(glm::vec3(0.5f));
        }
        if (m_WalkEmitter != nullptr)
        {
            m_WalkEmitter->SetLoop(false);
        }
    }
    else if (m_Owner->Name.find("bear") != std::string::npos)
    {
        m_Indicator->transform->Position = glm::vec3(5.0f, -8.4f, 0.0f);
        m_Indicator->transform->Scale = glm::vec3(20.0f);
        AssignBearTexture();
        if (m_RB != nullptr)
        {
            m_RB->PrepareInit(glm::vec3(2.f));
        }
    }
    else if (m_Owner->Name.find("skunk") != std::string::npos)
    {
        m_Indicator->transform->Position = glm::vec3(0.0f, -4.5f, 2.5f);
        m_Indicator->transform->Scale = glm::vec3(12.0f);
        if (m_RB != nullptr)
        {
            m_RB->PrepareInit(glm::vec3(1.f));
        }
        ParticleEmitter* emitter = m_Owner->AddComponent<ParticleEmitter>();
        emitter->Initialize("res/shaders/basicParticles.vert", "res/shaders/basicParticles.frag", "res/shaders/basicParticles.comp", "res/models/primitives/plane.obj", "res/textures/UI/bubble.png");
        emitter->SetSpawnRate(10);
        emitter->SetBulk(3);
        emitter->MaxVelocity = glm::vec3(1.f, 3.f, 1.f);
        emitter->MaxLife = 0.9f;
        emitter->Color = glm::vec4(0.211f, 0.578f, 0.234f, 1.f);
        emitter->Play();
    }
    else
    {
        // Failsafe
	    m_Indicator->transform->Scale = glm::vec3(2.0f, 2.0f, 2.0f);
        if (m_RB != nullptr)
        {
            m_RB->PrepareInit(glm::vec3(1.f));
        }
    }
    SetIndicatorShader(m_AssetMgr->PieChartShader);


    m_ProgressBar = m_SceneMgr->Instantiate(m_Owner, "res/models/primitives/plane.obj", m_AssetMgr->ProgressBarShader);
    m_ProgressBar->Name = "ProgressBar";
    m_ProgressBar->transform->Position = glm::vec3(0.f, 10.0f, 0.f);
    m_ProgressBar->transform->EulerAngles = glm::vec3(90.0f, 0.0f, 0.0f);
    m_ProgressBar->transform->Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ProgressBar->SetActive(false);
    SetProgressBarShader(m_AssetMgr->ProgressBarShader);


    m_Checkmark = m_SceneMgr->Instantiate(m_Owner, "res/models/primitives/plane.obj", m_AssetMgr->WorldUIShader);
    m_Checkmark->Name = "Checkmark";
    m_Checkmark->transform->Position = glm::vec3(0.f, 15.0f, 0.f);
    m_Checkmark->transform->EulerAngles = glm::vec3(90.0f, 0.0f, 0.0f);
    m_Checkmark->transform->Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    Model* checkmarkModel = m_Checkmark->GetComponent<Model>();
    if (checkmarkModel != nullptr)
    {
        checkmarkModel->ReassignShader(*m_AssetMgr->WorldUIShader);
        checkmarkModel->AssignTexture(*m_AssetMgr->GetTexture("res/textures/UI/checkmark.png"));
    }
    m_Checkmark->SetActive(false);
    SetCheckmarkShader(m_AssetMgr->WorldUIShader);

    std::shared_ptr<Shader> interactionShader = m_AssetMgr->GetNewWorldUIShader();
    m_InteractionMark = m_SceneMgr->Instantiate(m_Owner, "res/models/primitives/plane.obj", interactionShader);
    InteractionGlyph* glyph = m_InteractionMark->AddComponent<InteractionGlyph>();
    glyph->Initialize(interactionShader, *m_AssetMgr->GetTexture("res/textures/UI/x_button_color.png"));

    DrawRandomNeeds();

    m_PlayersInScene = m_SceneMgr->GetLevelParent()->FindComponentsInChildren<Player>();
}

void Animal::Start()
{
	Behaviour::Start();
}

void Animal::DrawUpdate()
{
	Behaviour::DrawUpdate();
	UpdateIndicatorColors();
    UpdateProgressBar();
}

void Animal::PickNewTargetPosition()
{
	float angle = Random::GetRandomFloat(0.0f, 2.0f * glm::pi<float>());
	float radius = Random::GetRandomFloat(2.0f, m_MovingRadius);

	glm::vec3 currentPos = m_Owner->transform->GetGlobalPosition();
	m_TargetPosition = currentPos + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
	m_WasDroppedByPlayer = false;

    Animator* animator = m_Owner->GetComponent<Animator>();

    m_StateController.RequestStateChange.Invoke(AnimalState::Walking);
}

void Animal::ForceNewTargetPosition()
{
    if (m_Owner && m_Owner->transform)
    {
		m_LastPosition = m_Owner->transform->GetGlobalPosition();
		m_CurrentAngle = m_Owner->transform->EulerAngles.y;
    }

	m_StuckTimer = 0.0f;
	m_CurrentWaitTime = 0.0f;
	m_JumpTimer = 0.0f;
	m_ShouldTeleport = false;

	PickNewTargetPosition();
}

std::vector<AnimalNeeds> Animal::GetRequiredServices()
{
    return m_RequiredServices;
}

void Animal::EnterTable(GameObject* table)
{
    if (table == nullptr) return;

    glm::vec3 tablePos = table->GetWorldPosition();
    m_TeleportTarget = tablePos + glm::vec3(0.0f, 2.0f, 0.0f);

    m_ShouldTeleport = true;

    RigidBody* rb = m_Owner->GetComponent<RigidBody>();
}

void Animal::Update()
{
    Behaviour::Update();
    m_CurrTime = m_SceneMgr->GetTimeLeft();

    glm::vec3 cameraPos = m_MainCamera->GetPosition();
    glm::vec3 animalPos = m_Owner->transform->GetGlobalPosition();
    float distance = glm::length(cameraPos - animalPos);

    if (distance < 140.0f)
    {
        Animator* animator = m_Owner->GetComponent<Animator>();
        if (animator != nullptr)
        {
            animator->UpdateAnimation(Time::GetDeltaTime());
        }
    }
    if (m_ShouldTeleport)
    {
        m_ShouldTeleport = false;

        m_Owner->transform->Position = m_TeleportTarget;

        RigidBody* rb = m_Owner->GetComponent<RigidBody>();
        if (rb != nullptr)
        {
            rb->SetLinearVelocity(glm::vec3(0.0f));

            rb->SetAngularVelocity(glm::vec3(0.0f));

            rb->Teleport(m_TeleportTarget);
        }
    }

    RigidBody* rb = m_Owner->GetComponent<RigidBody>();
    if (rb == nullptr) return;

    m_StateController.Update();
}


void Animal::UpdateIdle() 
{
    RigidBody* rb = m_Owner->GetComponent<RigidBody>();

    if (rb != nullptr)
    {
        rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
    }

    m_AnimalInteractions.Update(this);

    if (m_StateController.GetCurrentState() != AnimalState::Idle) return; //

    m_CurrentWaitTime += Time::GetDeltaTime();

    if (m_CurrentWaitTime >= m_WaitTime)
    {
        PickNewTargetPosition();
        m_CurrentWaitTime = 0.0f;

    }
}

void Animal::UpdateWalking() 
{
    RigidBody* rb = m_Owner->GetComponent<RigidBody>();
    if (rb == nullptr) return;

    m_AnimalInteractions.Update(this);
    if (m_StateController.GetCurrentState() != AnimalState::Walking) return;

    glm::vec3 currentPos = m_Owner->transform->GetGlobalPosition();

    float distanceMoved = glm::length(currentPos - m_LastPosition);
    if (distanceMoved < 0.05f)
    {
        m_StuckTimer += Time::GetDeltaTime();
    }
    else
    {
        m_StuckTimer = 0.0f;
    }

    m_LastPosition = currentPos;
    if (m_StuckTimer > 1.0f)
    {
        m_WaitTime = 1.0f;
        m_CurrentWaitTime = 0.0f;
        m_StuckTimer = 0.0f;
        m_JumpTimer = 0.0f;
        m_StateController.RequestStateChange.Invoke(AnimalState::Idle);
        return;
    }

    glm::vec3 diff = m_TargetPosition - currentPos;
    diff.y = 0.0f;
    float distance = glm::length(diff);

    if (distance > 0.5f)
    {
        glm::vec3 direction = glm::normalize(diff);


        float targetAngle = glm::degrees(atan2(direction.x, direction.z));
        targetAngle -= 90.0f;

        float deltaAngle = targetAngle - m_CurrentAngle;

        while (deltaAngle > 180.0f) deltaAngle -= 360.0f;
        while (deltaAngle < -180.0f) deltaAngle += 360.0f;

        m_CurrentAngle += deltaAngle * m_RotationSpeed * Time::GetDeltaTime();

        rb->SetRotation(glm::vec3(0.0f, m_CurrentAngle, 0.0f));

        glm::vec3 rayStart = currentPos + glm::vec3(0.0f, 0.5f, 0.0f);
        float lookAheadDistance = 1.5f;

        GameObject* obstacle = Engine::GetInstance().GetPhysicsEngine().CastRay(rayStart, direction, lookAheadDistance, rb->GetBodyID());

        if (obstacle != nullptr)
        {
            m_WaitTime = Random::GetRandomFloat(1.0f, 3.0f);
            m_CurrentWaitTime = 0.0f;
            m_StuckTimer = 0.0f;
            m_StateController.RequestStateChange.Invoke(AnimalState::Idle);
            return;
        }

        float speedMultiplier = 1.0f;
        float slowDownDistance = 2.0f;

        if (distance < slowDownDistance)
        {
            speedMultiplier = std::max(0.3f, distance / slowDownDistance);
        }

        glm::vec3 currentVelocity = rb->GetLinearVelocity();
        float currentMoveSpeed = m_MoveSpeed * speedMultiplier;
        float newVelY = currentVelocity.y;

        bool isOnGround = (currentPos.y <= -36.5f) || (std::abs(currentVelocity.y) < 0.1f);
        
        if (!isOnGround && m_Owner->Name.find("bunny") == std::string::npos)
        {
            currentMoveSpeed = 0.0f;
        }

        if (m_Owner->Name.find("bunny") != std::string::npos)
        {
            m_JumpTimer += Time::GetDeltaTime();

            if (isOnGround)
            {
                currentMoveSpeed *= 0.1f;
                if (m_JumpTimer >= 0.6f && distance > 1.5f)
                {
                    newVelY = m_JumpForce;
                    m_JumpTimer = 0.0f;
                    if (m_WalkEmitter != nullptr)
                    {
                        m_WalkEmitter->Play();
                    }
                }
            }
            else
            {
                currentMoveSpeed *= 1.5f;
            }
        }
        glm::vec3 targetVelocity = direction * currentMoveSpeed;

        float accel = (m_Owner->Name.find("bunny") != std::string::npos) ? m_Acceleration * 4.0f : m_Acceleration;

        if (distance < slowDownDistance)
        {
            accel *= 4.0f;
        }

        float newVelX = glm::mix(currentVelocity.x, targetVelocity.x, accel * Time::GetDeltaTime());
        float newVelZ = glm::mix(currentVelocity.z, targetVelocity.z, accel * Time::GetDeltaTime());

        rb->SetLinearVelocity(glm::vec3(newVelX, newVelY, newVelZ));
    }
    else
    {
        m_WaitTime = Random::GetRandomFloat(1.0f, 3.0f);
        m_CurrentWaitTime = 0.0f;
        m_StuckTimer = 0.0f;
        m_JumpTimer = 0.0f;
        m_StateController.RequestStateChange.Invoke(AnimalState::Idle);
    }
}

void Animal::UpdateChasing()
{
    m_AnimalInteractions.Update(this);
}

void Animal::UpdateEating()
{
    RigidBody* rb = m_Owner->GetComponent<RigidBody>();
}
void Animal::UpdatePickedUp() {
}

void Animal::UpdateThrow() {
}

void Animal::UpdateCheckIn() {
}

void Animal::UpdateFulfillingNeed()
{
    if (m_RB != nullptr)
    {
        m_RB->SetLinearVelocity(glm::vec3(0.0f));
        m_RB->SetAngularVelocity(glm::vec3(0.0f));
    }

    float dt = Time::GetDeltaTime();
    if (IsCurrentNeedInteractible())
    {
        m_CurrentNeedProgress -= m_SatisfactionDecreaseSpeed * dt;
        if (m_CurrentNeedProgress < 0)
        {
            m_CurrentNeedProgress = 0;
        }
    }
    else
    {
        m_CurrentNeedProgress += m_SatisfactionSpeed * dt;
    }

    UpdateProgressBar();

    if (m_CurrentNeedProgress >= 1.0f)
    {
        m_CurrentNeedProgress = 0.0f;

        StopFulfillingNeed();
        FulfillNeed(m_CurrentNeedBeingFulfilled);
    }

}


void Animal::SetIndicatorShader(std::shared_ptr<Shader> pieShader)
{
    m_PieShader = pieShader;
}

void Animal::SetProgressBarShader(std::shared_ptr<Shader> barShader)
{
    m_ProgressBarShader = barShader;
}

void Animal::SetCheckmarkShader(std::shared_ptr<Shader> checkmarkShader)
{
    m_CheckmarkShader = checkmarkShader;
}

void Animal::UpdateIndicatorColors()
{
    if (m_PieShader == nullptr) return;

    m_PieShader->Use();

    int numNeeds = m_RequiredServices.size();
    m_PieShader->SetInt("u_NumNeeds", numNeeds);
    m_PieShader->SetInt("u_IsHollow", 0);
    m_PieShader->SetInt("u_IsSquare", 0);

    glm::ivec4 shaderNeeds(-1, -1, -1, -1);

    for (int i = 0; i < numNeeds; i++)
    {
        int needValue = static_cast<int>(m_RequiredServices[i]);

        if (i == 0) shaderNeeds.x = needValue;
        if (i == 1) shaderNeeds.y = needValue;
        if (i == 2) shaderNeeds.z = needValue;
        if (i == 3) shaderNeeds.w = needValue;
    }

    m_PieShader->SetIVec4("u_Needs", shaderNeeds.x, shaderNeeds.y, shaderNeeds.z, shaderNeeds.w);
}

void Animal::FulfillNeed(AnimalNeeds need) {
    auto it = std::find(m_RequiredServices.begin(), m_RequiredServices.end(), need);

    if (it != m_RequiredServices.end())
    {       
        m_RequiredServices.erase(it);

        UpdateIndicatorColors();

        for (Player* player : m_PlayersInScene)
        {
            if (player != nullptr)
            {
                player->RecalculateBestTarget();
            }
        }

        if (m_RequiredServices.empty())
        {
            UpdateCheckmark();
            m_Checkmark->SetActive(true);

            if (m_Indicator != nullptr)
            {
                m_Indicator->SetActive(false);
            }
        }
    }
}

void Animal::PlayerFulfillNeed()
{
    if (m_StateController.GetCurrentState() == AnimalState::Rest)
    {
        if (IsCurrentNeedInteractible() && m_CurrentNeedProgress < 1)
        {
            m_CurrentNeedProgress += m_PlayerFulfillSpeed;
            m_InteractionEmitter->Play();
        }
    }
}

bool Animal::IsCurrentNeedInteractible()
{
    return m_CurrentNeedBeingFulfilled == AnimalNeeds::Massage;
}

void Animal::EnterPosition(glm::vec3 exactWorldPosition)
{
    m_TeleportTarget = exactWorldPosition;
    m_ShouldTeleport = true;
}

void Animal::DrawRandomNeeds()
{
	m_RequiredServices.clear();
	float progress = (m_TimeLimit - m_CurrTime) / m_TimeLimit;
	int min = lerp(m_minNeeds, m_maxNeeds - 1, progress);
	int max = lerp(m_minNeeds, m_maxNeeds, progress * 1.3f);
	m_numberOfNeeds = Random::GetRandomInt(min, max);
	std::vector<int> possibleNeeds = { 0, 1, 2, 3 };

	Random::Shuffle(possibleNeeds);

	for (int i = 0; i < m_numberOfNeeds; i++)
	{
		int randomType = possibleNeeds[i];
		m_RequiredServices.push_back(static_cast<AnimalNeeds>(randomType));
	}

	UpdateIndicatorColors();
}

void Animal::StartFulfillingNeed(AnimalNeeds need)
{
    m_ProgressBar->SetActive(true);
    m_StateController.RequestStateChange.Invoke(AnimalState::Rest);
    m_CurrentNeedBeingFulfilled = need;
    m_CurrentNeedProgress = 0.0f;
    if (IsCurrentNeedInteractible())
    {
        m_InteractionMark->SetActive(true);
    }
}

void Animal::StopFulfillingNeed()
{
    m_ProgressBar->SetActive(false);
    m_InteractionMark->SetActive(false);
    m_CurrentNeedProgress = 0.0f;
}

void Animal::UpdateProgressBar()
{
    if (m_ProgressBarShader != nullptr && m_ProgressBar != nullptr && m_StateController.GetCurrentState() == AnimalState::Rest)
    {
        m_ProgressBarShader->Use();
        m_ProgressBarShader->SetFloat("u_Progress", m_CurrentNeedProgress / 1);
        m_ProgressBarShader->SetVec3("cameraRight", m_MainCamera->GetRight());
        m_ProgressBarShader->SetVec3("cameraUp", m_MainCamera->GetUp());
    	m_ProgressBarShader->SetFloat("u_width", 3.0f);
    	m_ProgressBarShader->SetFloat("u_height", 0.35f);
    }
}

void Animal::UpdateCheckmark() {
    m_CheckmarkShader->Use();
    m_CheckmarkShader->SetVec3("cameraRight", m_MainCamera->GetRight());
    m_CheckmarkShader->SetVec3("cameraUp", -m_MainCamera->GetUp());
    m_CheckmarkShader->SetFloat("u_width", 3.0f);
    m_CheckmarkShader->SetFloat("u_height", 3.0f);
}

void Animal::ResetEverythingSpawn(glm::vec3 spawnPosition)
{
    m_Owner->transform->Position = spawnPosition;

    RigidBody* rb = m_Owner->GetComponent<RigidBody>();
    if (rb != nullptr)
    {
        rb->RequestTeleport(spawnPosition);

        rb->SetLinearVelocity(glm::vec3(0.0f));
        rb->SetAngularVelocity(glm::vec3(0.0f));
        rb->SetRotation(glm::vec3(0.0f));
    }

    m_CurrentAngle = 0.0f;
    m_LastPosition = spawnPosition;

    m_Checkmark->SetActive(false);
    m_Indicator->SetActive(true);
    DrawRandomNeeds();

    m_StateController.RequestStateChange.Invoke(AnimalState::CheckIn);

    float angle = Random::GetRandomFloat(0.0f, 2.0f * glm::pi<float>());
    float radius = Random::GetRandomFloat(0.0f, m_MovingRadius);
    m_TargetPosition = spawnPosition + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));

    m_IsInitialized = true;

    m_WaitTime = 1.5f;
    m_CurrentWaitTime = 0.0f;

    m_StuckTimer = 0.0f;
    m_JumpTimer = 0.0f;

    m_ShouldTeleport = false;
    m_CurrentNeedProgress = 0.0f;

    if (m_Owner->Name.find("bear") != std::string::npos) AssignBearTexture();
}

void Animal::AssignBearTexture() {
    int randomTextureIndex = Random::GetRandomInt(0, 5);
    std::string texturePath = "";

    switch (randomTextureIndex)
    {
    case 0:
        texturePath = "res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_White.png";
        break;
    case 1:
        texturePath = "res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_Black.png";
        break;
    case 2:
        texturePath = "res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_Brown.png";
        break;
    case 3:
        texturePath = "res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_Grey.png";
        break;
    case 4:
        texturePath = "res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_DarkBrown.png";
        break;
    case 5:
    default:
        texturePath = "res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor.png";
        break;
    }

    Model* model = m_Owner->GetComponent<Model>();
    if (model != nullptr)
    {
        model->AssignTexture(*m_AssetMgr->GetTexture(texturePath.c_str()));
    }
}

void Animal::AssignWalkEmitter()
{
    m_WalkEmitter = m_Owner->AddComponent<ParticleEmitter>();
    m_WalkEmitter->Initialize("res/shaders/basicParticles.vert", "res/shaders/basicParticles.frag", "res/shaders/basicParticles.comp", "res/models/primitives/plane.obj", "res/textures/UI/smoke.png");
    // m_WalkEmitter->SetSpawnRate(30);
    // m_WalkEmitter->SetBulk(1);
    // m_WalkEmitter->MaxVelocity = glm::vec3(0.f, 0.3f, 0.f);
    // m_WalkEmitter->MaxLife = 0.5f;
    // m_WalkEmitter->Color = glm::vec4(0.258f, 0.59f, 0.711f, .2f);
    // m_WalkEmitter->SetPositionOffset(glm::vec3(0, -6.2, 0));
    // m_WalkEmitter->IsRandomPosition = true;
    // m_WalkEmitter->RandomPositionOffset = glm::vec3(5, 0, 5);
    // m_WalkEmitter->Play();
}

void Animal::AssignLandEmitter()
{
}

void Animal::AssignInteractionEmitter()
{
    m_InteractionEmitter = m_Owner->AddComponent<ParticleEmitter>();
    m_InteractionEmitter->Initialize("res/shaders/basicParticles.vert", "res/shaders/basicParticles.frag", "res/shaders/basicParticles.comp", "res/models/primitives/plane.obj", "res/textures/UI/smoke.png");
    m_InteractionEmitter->SetSpawnRate(60);
    m_InteractionEmitter->SetBulk(20);
    m_InteractionEmitter->MaxVelocity = glm::vec3(3.f, .8f, 3.f);
    m_InteractionEmitter->MaxLife = 0.4f;
    // m_InteractionEmitter->Color = glm::vec4(0.258f, 0.59f, 0.711f, .2f);
    // m_InteractionEmitter->SetPositionOffset(glm::vec3(0, -6.2, 0));
    // m_InteractionEmitter->IsRandomPosition = true;
    // m_InteractionEmitter->RandomPositionOffset = glm::vec3(5, 0, 5);
    m_InteractionEmitter->SetLoop(false);
}
