#include "Animal.h"
#include "GameObject.h"
#include "Engine/Components/RigidBody.h"
#include <spdlog/spdlog.h>
#include <random>
#include <Random.h>
#include <Engine/Time.h>
#include <Engine/Engine.h>

#include "Engine/AssetManager.h"
#include <Engine/Animation/Animator.h>
#include "Player.h"

void Animal::Awake()
{
    Behaviour::Awake();
    
    m_AssetMgr = &Engine::GetInstance().GetAssetManager();
    m_SceneMgr = &Engine::GetInstance().GetGameManager().GetSceneManager();
	m_TimeLimit = m_SceneMgr->GetTimeLimit();
	m_CurrTime = m_SceneMgr->GetTimeLeft();
	m_MainCamera = m_SceneMgr->GetMainCamera().get();

    m_RB = m_Owner->AddComponent<RigidBody>();
	m_Indicator = m_SceneMgr->Instantiate(m_Owner, "res/models/PieChartPlane.obj", m_AssetMgr->PieChartShader);
	m_Indicator->Name = "NeedsIndicator";
	m_Indicator->transform->Position = glm::vec3(0.f, -0.8f, 0.f);
    // TODO add scripts for every animal kind, that will override enum
    if (m_Owner->Name.find("bunny") != std::string::npos)
    {
        m_Indicator->transform->Position = glm::vec3(0.f, -3.4f, 0.f);
        m_Indicator->transform->Scale = glm::vec3(8.0f);
        if (m_RB != nullptr)
        {
            m_RB->PrepareInit(glm::vec3(0.5f));
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


    m_ProgressBar = m_SceneMgr->Instantiate(m_Owner, "res/models/ProgressBarPlane.obj", m_AssetMgr->ProgressBarShader);
    m_ProgressBar->Name = "ProgressBar";
    m_ProgressBar->transform->Position = glm::vec3(0.f, 10.0f, 0.f);
    m_ProgressBar->transform->EulerAngles = glm::vec3(90.0f, 0.0f, 0.0f);
    m_ProgressBar->transform->Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    m_ProgressBar->SetActive(false);
    SetProgressBarShader(m_AssetMgr->ProgressBarShader);


    m_Checkmark = m_SceneMgr->Instantiate(m_Owner, "res/models/CheckmarkPlane.obj", m_AssetMgr->WorldUIShader);
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

    DrawRandomNeeds();

    m_PlayersInScene = m_SceneMgr->GetLevelParent()->FindComponentsInChildren<Player>();

    m_EventBinder.Bind(m_StateController.OnStateChanged, [this](AnimalState oldState, AnimalState newState)
    {
            Animator* animator = m_Owner->GetComponent<Animator>();

            if (newState == AnimalState::Idle)
            {
                PickNewTargetPosition();
                if (animator) animator->PlayAnimation("idle");
            }
            else if (newState == AnimalState::PickedUp)
            {
                if (animator) animator->PlayAnimation("idle");
            }
            else if (newState == AnimalState::Throw)
            {
                if (animator) animator->PlayAnimation("idle");
            }
            else if (newState == AnimalState::Rest)
            {
                if (animator) animator->PlayAnimation("idle");
            }
            else if (newState == AnimalState::CheckIn)
            {
                if (animator) animator->PlayAnimation("idle");
            }  
    });

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

    if (animator) animator->PlayAnimation("walk");
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

    Animator* animator = m_Owner->GetComponent<Animator>();
    if (animator != nullptr)
    {
        animator->UpdateAnimation(Time::GetDeltaTime());
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


void Animal::UpdateIdle() {
    RigidBody* rb = m_Owner->GetComponent<RigidBody>();

    m_AnimalInteractions.Update(this);

    glm::vec3 currentPos = m_Owner->transform->GetGlobalPosition();

    float distanceMoved = glm::length(currentPos - m_LastPosition);
    if (distanceMoved < 0.01f)
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
        rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
        PickNewTargetPosition();
        m_WaitTime = 1.0f;
        m_CurrentWaitTime = 0.0f;
        m_StuckTimer = 0.0f;
        m_JumpTimer = 0.0f;
        return;
    }
    glm::vec3 diff = m_TargetPosition - currentPos;
    diff.y = 0.0f;
    float distance = glm::length(diff);

    if (distance > 0.5f)
    {
        glm::vec3 direction = glm::normalize(diff);

        float targetAngle = glm::degrees(atan2(direction.x, direction.z));
        float deltaAngle = targetAngle - m_CurrentAngle;

        while (deltaAngle > 180.0f) deltaAngle -= 360.0f;
        while (deltaAngle < -180.0f) deltaAngle += 360.0f;

        m_CurrentAngle += deltaAngle * m_RotationSpeed * Time::GetDeltaTime();

        while (m_CurrentAngle > 360.0f) m_CurrentAngle -= 360.0f;
        while (m_CurrentAngle < -360.0f) m_CurrentAngle += 360.0f;

        rb->SetRotation(glm::vec3(0.0f, m_CurrentAngle, 0.0f));

        glm::vec3 rayStart = currentPos + glm::vec3(0.0f, 0.5f, 0.0f);
        float lookAheadDistance = 1.5f;

        GameObject* obstacle = Engine::GetInstance().GetPhysicsEngine().CastRay(rayStart, direction, lookAheadDistance, rb->GetBodyID());

        if (obstacle != nullptr)
        {
            rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
            PickNewTargetPosition();
            m_WaitTime = Random::GetRandomFloat(1.0f, 3.0f);
            m_CurrentWaitTime = 0.0f;
            m_StuckTimer = 0.0f;
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

        bool isOnGround = (currentPos.y <= -36.5f);
        
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
                }
            }
            else
            {
                currentMoveSpeed *= 1.5f;
            }
        }
        glm::vec3 targetVelocity = direction * (m_MoveSpeed * speedMultiplier);

        float accel = (m_Owner->Name.find("bunny") != std::string::npos) ? m_Acceleration * 4.0f : m_Acceleration;
        float newVelX = glm::mix(currentVelocity.x, targetVelocity.x, accel * Time::GetDeltaTime());
        float newVelZ = glm::mix(currentVelocity.z, targetVelocity.z, accel * Time::GetDeltaTime());

        rb->SetLinearVelocity(glm::vec3(newVelX, newVelY, newVelZ));
    }
    else
    {
        rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));

        m_CurrentWaitTime += Time::GetDeltaTime();

        if (m_CurrentWaitTime >= m_WaitTime)
        {
            PickNewTargetPosition();

            m_WaitTime = Random::GetRandomFloat(1.0f, 3.0f);
            m_CurrentWaitTime = 0.0f;
            m_StuckTimer = 0.0f;
            m_JumpTimer = 0.0f;
        }
    }
}

void Animal::UpdatePickedUp() {
}

void Animal::UpdateThrow() {
}

void Animal::UpdateCheckIn() {
}

void Animal::UpdateFulfillingNeed() {

    RigidBody* rb = m_Owner->GetComponent<RigidBody>();
    if (rb != nullptr)
    {
        rb->SetLinearVelocity(glm::vec3(0.0f));
        rb->SetAngularVelocity(glm::vec3(0.0f));
    }

    m_CurrentNeedProgress += m_SatisfactionSpeed * Time::GetDeltaTime();

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
}

void Animal::StopFulfillingNeed()
{
    m_ProgressBar->SetActive(false);
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