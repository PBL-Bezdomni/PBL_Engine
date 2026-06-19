#pragma once
#include "Engine/Components/Behaviour.h"
#include <vector>
#include <mutex>
#include <glm/glm.hpp>

class GameObject;

class River : public Behaviour
{
public:
    glm::vec3 FlowVelocity = glm::vec3(0.0f, 0.0f, 2.0f);

    void Update() override;

    void OnTriggerEnter(GameObject* obj);
    void OnTriggerExit(GameObject* obj);

private:
    std::vector<GameObject*> m_ObjectsInWater;

    std::vector<GameObject*> m_ObjectsToAdd;
    std::vector<GameObject*> m_ObjectsToRemove;

    std::mutex m_RiverMutex;
};