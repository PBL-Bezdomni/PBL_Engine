#include "River.h"
#include "GameObject.h"
#include "Engine/Components/RigidBody.h"
#include "Game/Scripts/Animal.h"
#include <algorithm>

void River::Update()
{
    Behaviour::Update();

    std::lock_guard<std::mutex> lock(m_RiverMutex);

    for (GameObject* obj : m_ObjectsToRemove)
    {
        auto it = std::find(m_ObjectsInWater.begin(), m_ObjectsInWater.end(), obj);
        if (it != m_ObjectsInWater.end())
        {
            m_ObjectsInWater.erase(it);

            AnimalStateController* animalState = obj->GetDerivedComponent<AnimalStateController>();
            if (animalState != nullptr) animalState->RequestStateChange.Invoke(AnimalState::None);

            RigidBody* rb = obj->GetComponent<RigidBody>();
            if (rb != nullptr) {
                rb->SetGhostMode(false);
            }
        }
    }
    m_ObjectsToRemove.clear();

    for (GameObject* obj : m_ObjectsToAdd)
    {
        if (std::find(m_ObjectsInWater.begin(), m_ObjectsInWater.end(), obj) == m_ObjectsInWater.end())
        {
            m_ObjectsInWater.push_back(obj);

            RigidBody* rb = obj->GetComponent<RigidBody>();
            if (rb != nullptr) {
                rb->SetGhostMode(true);
            }
        }
    }
    m_ObjectsToAdd.clear();

    for (GameObject* obj : m_ObjectsInWater)
    {
        if (obj == nullptr) continue;

        RigidBody* rb = obj->GetComponent<RigidBody>();
        if (rb != nullptr)
        {
            rb->SetLinearVelocity(FlowVelocity);
        }
    }
}

void River::OnTriggerEnter(GameObject* obj)
{
    if (obj == nullptr) return;

    std::lock_guard<std::mutex> lock(m_RiverMutex);
    m_ObjectsToAdd.push_back(obj);
}

void River::OnTriggerExit(GameObject* obj)
{
    if (obj == nullptr) return;

    std::lock_guard<std::mutex> lock(m_RiverMutex);
    m_ObjectsToRemove.push_back(obj);
}