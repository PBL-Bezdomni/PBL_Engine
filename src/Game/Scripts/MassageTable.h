#pragma once
#include "Engine/Components/Behaviour.h"

class Animal;
class GameObject;

class MassageTable : public Behaviour
{
private:
    bool m_IsOccupied = false;
    GameObject* m_OccupyingAnimal = nullptr;

public:
    void OnTriggerEnter(GameObject* other) override;
};