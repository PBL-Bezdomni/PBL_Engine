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

void SpawnManager::Awake()
{
	Behaviour::Awake();
}

void SpawnManager::Start()
{
	Behaviour::Start();
	Engine* engine = &Engine::GetInstance();
	m_AssetMgr = &engine->GetAssetManager();
	m_SceneMgr = &engine->GetGameManager().GetSceneManager();
	m_AnimalParent = m_SceneMgr->GetLevelParent();
	CreateEntities(m_AssetMgr->BasicShader);

	ball = make_shared<GameObject>();
	Model ballModel = *m_AssetMgr->GetModel(*m_AssetMgr->BasicShader, "res/models/sphere/sphere.obj");
	ball->AddComponent<Model>(ballModel);
	ball->transform->Position = glm::vec3(0, 0, 0);
	m_Owner->AddChild(ball.get());
}

void SpawnManager::Update()
{
	Behaviour::Update();
	m_SpawnCounter += Time::GetDeltaTime();
	if (m_SpawnCounter >= m_SpawnTime)
	{
		m_SpawnCounter = 0;
		if (m_AnimalsPool.size() > 0)
		{
			shared_ptr<GameObject> animal = PickAnimal();
			SetSpawnValue(animal.get());
			SpawnAnimal(animal.get());
		}
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
	return animal;
}

void SpawnManager::SetSpawnValue(GameObject* animal)
{
	glm::vec3 spawnPosition = m_Owner->transform->GetGlobalPosition();
	//spawnedEntity->transform->Scale = glm::vec3(2, 2, 2);
	if (animal != nullptr)
	{
		animal->transform->Position = spawnPosition;
		animal->GetComponent<RigidBody>()->Teleport(spawnPosition);
		
		Animal* anScrpt = animal->GetComponent<Animal>(); 
		anScrpt->ForceNewTargetPosition();
		anScrpt->DrawRandomNeeds();
	}
}

void SpawnManager::SpawnAnimal(GameObject* animal)
{
	RigidBody* animalRB = animal->GetComponent<RigidBody>();
	glm::vec3 throwVelocity = glm::vec3(0, 0, -1) * 100.f;
	animalRB->SetLinearVelocity(throwVelocity);
}

shared_ptr<GameObject> SpawnManager::CreateAnimal(shared_ptr<Shader> shader, const char* path, const char* name, int index)
{
	shared_ptr<GameObject> animal = m_SceneMgr->Instantiate(m_AnimalParent, path, shader);
	animal->Name = name + std::to_string(index);
	animal->AddComponent<RigidBody>();
	animal->GetComponent<RigidBody>()->PrepareInit();
	animal->AddComponent<Animal>();
	animal->transform->Position = glm::vec3(1000, 5, -1000);
	animal->UpdateSelfAndChild();
	m_SceneMgr->AddAnimal(animal);
	return animal;
}

shared_ptr<GameObject> SpawnManager::CreateBunny(shared_ptr<Shader> shader, int index)
{
	return CreateAnimal(shader, "res/models/animals/bunny/bunny.obj", "bunny", index);
}

shared_ptr<GameObject> SpawnManager::CreateBear(shared_ptr<Shader> shader, int index)
{
	return CreateAnimal(shader, "res/models/animals/bear/bear_1500.fbx", "bear", index);
}

shared_ptr<GameObject> SpawnManager::CreateSkunk(shared_ptr<Shader> shader, int index)
{
	return CreateAnimal(shader, "res/models/animals/skunks/skunks.fbx", "skunk", index);
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
