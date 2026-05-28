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
	SpawnEntities(m_AssetMgr->BasicShader);
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
			int index = Random::GetRandomInt(0, m_AnimalsPool.size() - 1);
			if (index >= m_AnimalsPool.size())
			{
				spdlog::error("Animal spawner wanted bigger index, tweak random calculator");
				return;
			}
			
			GameObject* animal = m_AnimalsPool[index].get();
			swap(m_AnimalsPool[index], m_AnimalsPool.back());
			m_AnimalsPool.pop_back();
			
			float posX = Random::GetRandomInt(-WALL_X_BORDER, WALL_X_BORDER);
			float posY = Random::GetRandomInt(-WALL_Y_BORDER, WALL_Y_BORDER);
			//spawnedEntity->transform->Scale = glm::vec3(2, 2, 2);
			if (animal != nullptr)
			{
				animal->transform->Position = glm::vec3(posX, 5, posY);
				animal->GetComponent<RigidBody>()->Teleport(animal->transform->Position);
				Animal* anScrpt = animal->GetComponent<Animal>(); 
				anScrpt->ForceNewTartetPosition();
				anScrpt->DrawRandomNeeds();
			}
		}
	}
}

shared_ptr<GameObject> SpawnManager::SpawnAnimal(shared_ptr<Shader> shader, const char* path, const char* name, int index)
{
	shared_ptr<GameObject> animal = m_SceneMgr->Instantiate(m_AnimalParent, path, shader);
	animal->Name = name + std::to_string(index);
	animal->AddComponent<RigidBody>();
	animal->GetComponent<RigidBody>()->PrepareInit();
	animal->AddComponent<Animal>();
	animal->transform->Position = glm::vec3(1000, 5, 1000);
	animal->UpdateSelfAndChild();
	m_SceneMgr->AddAnimal(animal);
	return animal;
}

shared_ptr<GameObject> SpawnManager::SpawnBunny(shared_ptr<Shader> shader, int index)
{
	return SpawnAnimal(shader, "res/models/animals/bunny/bunny.obj", "bunny", index);
}

shared_ptr<GameObject> SpawnManager::SpawnBear(shared_ptr<Shader> shader, int index)
{
	return SpawnAnimal(shader, "res/models/animals/bear/bear_1500.fbx", "bear", index);
}

shared_ptr<GameObject> SpawnManager::SpawnSkunk(shared_ptr<Shader> shader, int index)
{
	return SpawnAnimal(shader, "res/models/animals/skunks/skunks.fbx", "skunk", index);
}

void SpawnManager::SpawnEntities(shared_ptr<Shader> shader)
{
	int r = Random::GetRandomInt(0, 99);

	for (int i = 0; i < m_BunnyLimit; i++)
	{
		m_AnimalsPool.push_back(SpawnBunny(shader, i + 1));
	}
	for (int i = 0; i < m_BearLimit; i++)
	{
		m_AnimalsPool.push_back(SpawnBear(shader, i + 1));
	}
	for (int i = 0; i < m_SkunkLimit; i++)
	{
		m_AnimalsPool.push_back(SpawnSkunk(shader, i + 1));
	}
}
