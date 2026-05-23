#include "Animal.h"
#include "GameObject.h"
#include "Engine/Components/RigidBody.h"
#include <spdlog/spdlog.h>
#include <random>
#include <Engine/Time.h>

void Animal::Awake()
{
	Behaviour::Awake();
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> amountDist(1, 3);
	std::uniform_int_distribution<std::mt19937::result_type> typeDist(0, 3);

	m_numberOfNeeds = amountDist(rng);

	for (int i = 0; i < m_numberOfNeeds; i++)
	{
		int randomType = typeDist(rng);
		m_RequiredServices.push_back(static_cast<AnimalNeeds>(randomType));
	}
}

void Animal::Start()
{
	Behaviour::Start();
}

void Animal::PickNewTargetPosition()
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * glm::pi<float>());
	std::uniform_real_distribution<float> radiusDist(0.0f, m_MovingRadius);

	float angle = angleDist(rng);
	float radius = radiusDist(rng);

	glm::vec3 currentPos = m_Owner->transform->Position;
	m_TargetPosition = currentPos + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
	m_IsMoving = true;
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

    if (!m_IsInitialized)
    {
		m_LastPosition = m_Owner->transform->Position;
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
	if (m_IsSeated) return;

	RigidBody* rb = m_Owner->GetComponent<RigidBody>();

	if (rb == nullptr) return;

    if (m_IsMoving)
    {
        glm::vec3 currentPos = m_Owner->transform->Position;

        float distanceMoved = glm::length(currentPos - m_LastPosition);
        if (distanceMoved < 0.2f * Time::GetDeltaTime() * m_MoveSpeed)
        {
            m_StuckTimer += Time::GetDeltaTime();
        }
        else
        {
            m_StuckTimer = 0.0f;
        }

        m_LastPosition = currentPos;
        if (m_StuckTimer > 0.2f)
        {
            m_IsMoving = false;
            rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));
            m_WaitTime = 1.0f;
            m_CurrentWaitTime = 0.0f;
            m_StuckTimer = 0.0f;
            return;
        }
        glm::vec3 diff = m_TargetPosition - currentPos;
        diff.y = 0.0f;
        float distance = glm::length(diff);

        if (distance > 0.5f)
        {
            glm::vec3 direction = glm::normalize(diff);

            float targetAngle = glm::degrees(atan2(direction.x, direction.z));
            rb->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));

            glm::vec3 currentVelocity = rb->GetLinearVelocity();
            glm::vec3 targetVelocity = direction * m_MoveSpeed;
            rb->SetLinearVelocity(glm::vec3(targetVelocity.x, currentVelocity.y, targetVelocity.z));
        }
        else
        {
            m_IsMoving = false;
            rb->SetLinearVelocity(glm::vec3(0.0f, rb->GetLinearVelocity().y, 0.0f));

            std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> waitDist(1.0f, 3.0f);
            m_WaitTime = waitDist(rng);
            m_CurrentWaitTime = 0.0f;
            m_StuckTimer = 0.0f;
        }
	}
	else
	{
		m_LastPosition = m_Owner->transform->Position;

		m_CurrentWaitTime += Time::GetDeltaTime();
		if (m_CurrentWaitTime >= m_WaitTime)
		{
			PickNewTargetPosition();
		}
	}
}