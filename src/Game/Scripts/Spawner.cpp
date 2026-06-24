#include "Spawner.h"
#include "Game/Scripts/Animal.h"

void Spawner::Initialize(int id)
{
	if (id == 0)
	{
		m_SpawnPositionOffset = glm::vec3(-5, 0, 0);
		m_SpawnVelocity = glm::vec3(-1, 0, 0);
	}
	else if (id == 1)
	{
		m_SpawnPositionOffset = glm::vec3(0, 0, -5);
		m_SpawnVelocity = glm::vec3(0, 0, -1);
	}
	else if (id == 2)
	{
		m_SpawnPositionOffset = glm::vec3(5, 0, 0);
		m_SpawnVelocity = glm::vec3(1, 0, 0);
	}
}

glm::vec3 Spawner::GetSpawnPosition()
{
	return m_Owner->transform->GetGlobalPosition() + m_SpawnPositionOffset;
}

glm::vec3 Spawner::GetSpawnerVelocity()
{
	return m_SpawnVelocity;
}

void Spawner::OnTriggerEnter(GameObject* other)
{
	Behaviour::OnCollisionEnter(other);
	Animal* animal = other->GetComponent<Animal>();
	if (animal != nullptr)
	{
		OnAnimalEntered.Invoke(animal);
	}
}
