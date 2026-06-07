#include "Animal.h"
#include "GameObject.h"
#include "Engine/Components/RigidBody.h"
#include <spdlog/spdlog.h>
#include <random>
#include <Random.h>
#include <Engine/Time.h>
#include <Engine/Engine.h>

#include "Engine/AssetManager.h"
#include "Engine/Engine.h"

void Animal::Awake()
{
    Behaviour::Awake();
    
    m_AssetMgr = &Engine::GetInstance().GetAssetManager();
    m_SceneMgr = &Engine::GetInstance().GetGameManager().GetSceneManager();
	m_TimeLimit = m_SceneMgr->GetTimeLimit();
	m_CurrTime = m_SceneMgr->GetTimeLeft();

	m_Indicator = m_SceneMgr->Instantiate(m_Owner, "res/models/PieChartPlane.obj", m_AssetMgr->PieChartShader);
	m_Indicator->Name = "NeedsIndicator";
	m_Indicator->transform->Position = glm::vec3(0.f, -0.8f, 0.f);
	m_Indicator->transform->Scale = glm::vec3(2.0f, 2.0f, 2.0f);	
    SetIndicatorShader(m_AssetMgr->PieChartShader);

    m_ProgressBar = m_SceneMgr->Instantiate(m_Owner, "res/models/ProgressBarPlane.obj", m_AssetMgr->ProgressBarShader);
    m_ProgressBar->Name = "ProgressBar";
    m_ProgressBar->transform->Position = glm::vec3(0.f, 5.0f, 0.f);
    m_ProgressBar->transform->EulerAngles = glm::vec3(90.0f, 0.0f, 0.0f);
    m_ProgressBar->transform->Scale = glm::vec3(0.0f);
    SetProgressBarShader(m_AssetMgr->ProgressBarShader);

    DrawRandomNeeds();
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
	m_IsMoving = true;
	m_WasDroppedByPlayer = false;
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
	m_IsSeated = false;
	m_ShouldTeleport = false;

	PickNewTargetPosition();
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

    if (!m_IsInitialized)
    {
		m_LastPosition = m_Owner->transform->GetGlobalPosition();
		m_CurrentAngle = m_Owner->transform->EulerAngles.y;
		PickNewTargetPosition();
		m_IsInitialized = true;
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

    if (m_IsSeatedInObject) {

        RigidBody* rb = m_Owner->GetComponent<RigidBody>();
        if (rb != nullptr)
        {
            rb->SetLinearVelocity(glm::vec3(0.0f));
            rb->SetAngularVelocity(glm::vec3(0.0f));
        }

        if (m_IsFulfillingNeed)
        {
            m_CurrentNeedProgress += m_SatisfactionSpeed * Time::GetDeltaTime();

            m_ProgressBar->transform->Scale = glm::vec3(1.5f, 1.0f, 0.3f);

            UpdateProgressBar();

            if (m_CurrentNeedProgress >= 1.0f)
            {
                m_IsFulfillingNeed = false;
                m_CurrentNeedProgress = 0.0f;

                FulfillNeed(m_CurrentNeedBeingFulfilled);
            }

            return;
        }
    }

    if (m_IsSeated) return;

	m_AnimalInteractions.Update(this);

	RigidBody* rb = m_Owner->GetComponent<RigidBody>();

	if (rb == nullptr) return;

    if (m_IsMoving)
    {
        glm::vec3 currentPos = m_Owner->transform->GetGlobalPosition();

        float distanceMoved = glm::length(currentPos - m_LastPosition);
        if (distanceMoved < 0.1f * Time::GetDeltaTime() * m_MoveSpeed)
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
            m_IsMoving = false;
            rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
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
				m_IsMoving = false;
				rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));

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

            bool isOnGround = (currentPos.y <= 2.6f);

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
            float newVelX = glm::mix(currentVelocity.x, targetVelocity.x, accel *Time::GetDeltaTime());
            float newVelZ = glm::mix(currentVelocity.z, targetVelocity.z, accel * Time::GetDeltaTime());

            rb->SetLinearVelocity(glm::vec3(newVelX, newVelY, newVelZ));
        }
        else
        {
            m_IsMoving = false;
            rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
        
			m_WaitTime = Random::GetRandomFloat(1.0f, 3.0f);
            m_CurrentWaitTime = 0.0f;
            m_StuckTimer = 0.0f;
			m_JumpTimer = 0.0f;
        }
	}
	else
	{
		m_LastPosition = m_Owner->transform->GetGlobalPosition();

		m_CurrentWaitTime += Time::GetDeltaTime();
		if (m_CurrentWaitTime >= m_WaitTime)
		{
			PickNewTargetPosition();
		}
	}

    if (m_Indicator != nullptr)
    {
    	glm::vec3 worldPos = glm::vec3(m_Owner->transform->ModelMatrix[3]);
        // m_Indicator->transform->Position = worldPos + glm::vec3(0.0f, -0.5f, 0.0f);
        m_Indicator->UpdateSelfAndChild();
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

        if (m_RequiredServices.empty())
        {
            spdlog::info("Zwierzak zaspokoil wszystkie potrzeby");

            if (m_Indicator != nullptr)
            {
                m_Indicator->transform->Scale = glm::vec3(0.0f);
            }
        }
    }
}

void Animal::EnterPosition(glm::vec3 exactWorldPosition)
{
    m_TeleportTarget = exactWorldPosition;
    m_ShouldTeleport = true;
    m_IsSeated = true;
    m_IsMoving = false;
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
    m_IsSeatedInObject = true;
    m_IsFulfillingNeed = true;
    m_CurrentNeedBeingFulfilled = need;
    m_CurrentNeedProgress = 0.0f;
}

void Animal::StopFulfillingNeed()
{
    m_IsSeatedInObject = false; 
    m_IsFulfillingNeed = false;
    m_CurrentNeedProgress = 0.0f;
}

void Animal::UpdateProgressBar()
{
    if (m_ProgressBarShader != nullptr && m_ProgressBar != nullptr)
    {
        m_ProgressBarShader->Use();
        m_ProgressBarShader->SetFloat("u_Progress", m_CurrentNeedProgress);
    }
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

	float angle = Random::GetRandomFloat(0.0f, 2.0f * glm::pi<float>());
	float radius = Random::GetRandomFloat(0.0f, m_MovingRadius);
	m_TargetPosition = spawnPosition + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));

	m_IsMoving = false;
	m_IsInitialized = true;

	m_IsSeated = false;
	m_IsFulfillingNeed = false;

	m_WaitTime = 1.5f;
	m_CurrentWaitTime = 0.0f;
	
    m_StuckTimer = 0.0f;
	m_JumpTimer = 0.0f;

    m_ShouldTeleport = false;
	m_CurrentNeedProgress = 0.0f;
}