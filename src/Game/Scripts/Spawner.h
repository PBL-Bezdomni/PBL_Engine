#pragma once
#include "Engine/Components/Behaviour.h"
#include "Engine/Events/AEvent.h"
#include "glm/vec3.hpp"

class Animal;

class Spawner : public Behaviour
{
public:
	AEvent<Animal*> OnAnimalEntered;

	void Initialize(int id);
	glm::vec3 GetSpawnPosition();
	glm::vec3 GetSpawnerVelocity();
	void OnTriggerEnter(GameObject* other) override;
	
	const char* GetScriptName() const override { return "Spawner"; }
private:
	glm::vec3 m_SpawnPositionOffset;
	glm::vec3 m_SpawnVelocity;
};
