#pragma once
#include "Behaviour.h"
#include <vector>

class Player;

class TargetingZone : public Behaviour {
public:
    float BaseWeight = 100.0f; 
    Player* LinkedPlayer = nullptr;

    std::vector<GameObject*> AnimalsInZone;

    virtual void OnTriggerEnter(GameObject* other);
    virtual void OnTriggerExit(GameObject* other);

    float GetAnimalScore(GameObject* animal);
};