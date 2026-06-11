#include "SpawnManager.h"
#include "Engine/Loader.h"
#include <random>
// #include "Model.h"
#include "GameObject.h"
#include "Random.h"
#include "Shader.h"
#include "Engine/Engine.h"
#include "Engine/Time.h"
#include "Game/SceneManager.h"
#include "Engine/Components/RigidBody.h"
#include "Game/Scripts/Animal.h"
#include "Game/Scripts/AOnsenObject.h"
#include "Game/Scripts/TutorialArrow.h"

SpawnManager* SpawnManager::Instance = nullptr;

void SpawnManager::Awake()
{
	Behaviour::Awake();
	Instance = this;
}

void SpawnManager::Start()
{
	Behaviour::Start();
	Engine* engine = &Engine::GetInstance();
	m_AssetMgr = &engine->GetAssetManager();
	m_SceneMgr = &engine->GetGameManager().GetSceneManager();
	m_AnimalParent = m_SceneMgr->GetLevelParent();
	CreateEntities(m_AssetMgr->BasicShader);
}

void SpawnManager::Update()
{
	Behaviour::Update();
	m_SpawnCounter += Time::GetDeltaTime();
	if (m_SpawnCounter >= m_SpawnTime)
	{
		m_SpawnCounter = 0;
		if (m_AnimalsPool.size() > 0 && m_SpawnedAnimalsPool.size() < m_SpawnedLimit)
		{
			shared_ptr<GameObject> animal = PickAnimal();
			SetSpawnValue(animal.get());
			SpawnAnimal(animal.get());
		}
	}
}
void SpawnManager::AddMoney(int money)
{
	m_EarnedMoney += money;
}

void SpawnManager::OnTriggerEnter(GameObject* other)
{
	Behaviour::OnTriggerEnter(other);

	Animal* animal = other->GetComponent<Animal>();
	if (animal != nullptr)
	{
		if (animal->GetRequiredServices().empty())
		{
			Engine::GetInstance().GetAudioManager().PlaySound("res/audio/1.wav");
			AddMoney(10); 
		}
		DespawnAnimal(other);
	}
}
shared_ptr<GameObject> SpawnManager::PickAnimal()
{
	int index = Random::GetRandomInt(0, m_AnimalsPool.size() - 1);
	if (index >= m_AnimalsPool.size())
	{
		spdlog::error("Animal spawner wanted bigger index, tweak random calculator");
		return nullptr;
	}
			
	shared_ptr<GameObject> animal = m_AnimalsPool[index];
	swap(m_AnimalsPool[index], m_AnimalsPool.back());
	m_AnimalsPool.pop_back();
	m_SpawnedAnimalsPool.push_back(animal);
	return animal;
}

void SpawnManager::SetSpawnValue(GameObject* animal)
{
	glm::vec3 spawnPosition = m_Owner->transform->GetGlobalPosition();
	spawnPosition.x -= 5;
	//spawnedEntity->transform->Scale = glm::vec3(2, 2, 2);
	if (animal != nullptr)
	{
		animal->transform->Position = spawnPosition;
		animal->GetComponent<RigidBody>()->Teleport(spawnPosition);
		
		Animal* anScrpt = animal->GetComponent<Animal>(); 
		anScrpt->ResetEverythingSpawn(spawnPosition);
		anScrpt->DrawRandomNeeds();
	}
}

void SpawnManager::SpawnAnimal(GameObject* animal)
{
	RigidBody* animalRB = animal->GetComponent<RigidBody>();
	glm::vec3 throwVelocity = glm::vec3(-1, 0, 0) * 40.f;
	animalRB->SetLinearVelocity(throwVelocity);
}

void SpawnManager::DespawnAnimal(GameObject* animal)
{
	if (animal != nullptr)
	{
		for (int i = 0; i < m_SpawnedAnimalsPool.size(); i++)
		{
			shared_ptr<GameObject> spawnedAnimal = m_SpawnedAnimalsPool[i];
			if (spawnedAnimal.get() == animal)
			{
				swap(m_SpawnedAnimalsPool[i], m_SpawnedAnimalsPool.back());
				m_SpawnedAnimalsPool.pop_back();
				m_AnimalsPool.push_back(spawnedAnimal);
				// spawnedAnimal->transform->Position = m_ExiledPos;
				// spawnedAnimal->GetComponent<Animal>()->m_IsMoving = false;
				Animal* anScript = spawnedAnimal->GetComponent<Animal>();
				if (anScript != nullptr)
				{
					anScript->m_IsInitialized = false;
					anScript->ChangeState(AnimalState::None);
				}
				RigidBody* rb = spawnedAnimal->GetComponent<RigidBody>();
				if (rb != nullptr)
				{
					rb->RequestTeleport(m_ExiledPos);
				}
				return;
			}
		}
	}
}

shared_ptr<GameObject> SpawnManager::CreateAnimal(shared_ptr<Shader> shader, const char* path, const char* name, int index)
{
	shared_ptr<GameObject> animal = m_SceneMgr->Instantiate(m_AnimalParent, path, shader);
	animal->Name = name + std::to_string(index);
	// animal->AddComponent<RigidBody>();
	// animal->GetComponent<RigidBody>()->PrepareInit();
	animal->AddComponent<Animal>();
	animal->transform->Position = m_ExiledPos;
	animal->UpdateSelfAndChild();
	//animal->AddComponent<TutorialAnimal>();
	m_SceneMgr->AddAnimal(animal);
	return animal;
}

shared_ptr<GameObject> SpawnManager::CreateBunny(shared_ptr<Shader> shader, int index)
{
	return CreateAnimal(shader, "res/models/animals/bunny/Bunnyf.obj", "bunny", index);
}

shared_ptr<GameObject> SpawnManager::CreateBear(shared_ptr<Shader> shader, int index)
{
	return CreateAnimal(shader, "res/models/animals/bear/Bearf.obj", "bear", index);
}

shared_ptr<GameObject> SpawnManager::CreateSkunk(shared_ptr<Shader> shader, int index)
{
	return CreateAnimal(shader, "res/models/animals/skunks/Skunksf.obj", "skunk", index);
}


void SpawnManager::CreateEntities(shared_ptr<Shader> shader)
{
	int r = Random::GetRandomInt(0, 99);

	for (int i = 0; i < m_BunnyLimit; i++)
	{
		m_AnimalsPool.push_back(CreateBunny(shader, i + 1));
	}
	for (int i = 0; i < m_BearLimit; i++)
	{
		m_AnimalsPool.push_back(CreateBear(shader, i + 1));
	}
	for (int i = 0; i < m_SkunkLimit; i++)
	{
		m_AnimalsPool.push_back(CreateSkunk(shader, i + 1));
	}
}

