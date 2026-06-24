#include "TargetingZone.h"
#include "Game/Scripts/Player.h"
#include "Game/Scripts/Animal.h"
#include "Engine/Components/RigidBody.h"
#include <algorithm>

void TargetingZone::OnTriggerEnter(GameObject* other)
{
    Animal* animalScript = other->GetDerivedComponent<Animal>();
    if (animalScript != nullptr)
    {
        if (std::find(AnimalsInZone.begin(), AnimalsInZone.end(), other) == AnimalsInZone.end())
        {
            AnimalsInZone.push_back(other);

            if (LinkedPlayer != nullptr)
            {
                float score = GetAnimalScore(other);
                LinkedPlayer->OnAnimalEnteredZone(other, score);
            }
        }
    }
}

void TargetingZone::OnTriggerExit(GameObject* other)
{
    auto it = std::find(AnimalsInZone.begin(), AnimalsInZone.end(), other);
    if (it != AnimalsInZone.end())
    {
        AnimalsInZone.erase(it);

        if (LinkedPlayer != nullptr)
        {
            LinkedPlayer->OnAnimalExitedZone(other);
        }
    }
}

float TargetingZone::GetAnimalScore(GameObject* animal)
{
    float finalScore = BaseWeight;

    Animal* animalScript = animal->GetDerivedComponent<Animal>();

    if (animalScript->GetStateController()->GetCurrentState() == AnimalState::Throw)
    {
        finalScore += 1000.0f;
    }

    if (animalScript->m_ProgressBar->IsActive())
    {
        if (animalScript->IsCurrentNeedInteractible())
        {
            return -5;
        }
        else
        {
            return -2;
        }
    }

    return finalScore;
}