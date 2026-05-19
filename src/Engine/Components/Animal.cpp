#include "Animal.h"
#include "GameObject.h"
#include "Engine/Components/RigidBody.h"
#include <spdlog/spdlog.h>
#include <random>

void Animal::Init() {

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
}