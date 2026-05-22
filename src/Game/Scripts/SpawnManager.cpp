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
}

void SpawnManager::Update()
{
	Behaviour::Update();
	m_SpawnCounter += Time::GetDeltaTime();
	if (m_SpawnCounter >= m_SpawnTime)
	{
		m_SpawnCounter = 0;
		shared_ptr<GameObject> entity = SpawnEntity(m_AssetMgr->BasicShader);
		float posX = Random::GetRandomInt(-WALL_X_BORDER, WALL_X_BORDER);
		float posY = Random::GetRandomInt(-WALL_Y_BORDER, WALL_Y_BORDER);
		//spawnedEntity->transform->Scale = glm::vec3(2, 2, 2);
		if (entity != nullptr)
		{
			//entity->transform->Position = glm::vec3(posX, 5, posY);
			//entity->GetComponent<RigidBody>()->Teleport(glm::vec3(posX, 5, posY));
			// entity->SetParent(m_SceneMgr->objectsTransform.get());

			entity->UpdateSelfAndChild();
			entity->AddComponent<RigidBody>();
			entity->GetComponent<RigidBody>()->Init();
			entity->GetComponent<RigidBody>()->Teleport(glm::vec3(posX, 5, posY));
			entity->AddComponent<Animal>();
			m_SceneMgr->AddAnimal(entity);

		}
	}
}

shared_ptr<GameObject> SpawnManager::SpawnBunny(shared_ptr<Shader> shader)
{
	if (m_SpawnedBunnies.size() < m_BunnyLimit)
	{
		shared_ptr<GameObject> bunny = m_SceneMgr->Instantiate("res/models/animals/bunny/bunny.obj", shader);
		bunny->Name = "bunny";
		m_SpawnedEntities.push_back(bunny);
		m_SpawnedBunnies.push_back(bunny);
		return bunny;
	}
	return nullptr;
}

shared_ptr<GameObject> SpawnManager::SpawnBear(shared_ptr<Shader> shader)
{
	if (m_SpawnedBears.size() < m_BearLimit)
	{
		shared_ptr<GameObject> bear = m_SceneMgr->Instantiate("res/models/animals/bear/bear_1500.fbx", shader);
		bear->Name = "bear";
		m_SpawnedEntities.push_back(bear);
		m_SpawnedBears.push_back(bear);
		return bear;
	}
	return nullptr;
}

shared_ptr<GameObject> SpawnManager::SpawnSkunk(shared_ptr<Shader> shader)
{
	if (m_SpawnedSkunks.size() < m_SkunkLimit)
	{
		shared_ptr<GameObject> skunk = m_SceneMgr->Instantiate("res/models/animals/skunks/skunks.fbx", shader);
		skunk->Name = "skunk";
		m_SpawnedEntities.push_back(skunk);
		m_SpawnedSkunks.push_back(skunk);
		return skunk;
	}
	return nullptr;
}

shared_ptr<GameObject> SpawnManager::SpawnEntity(shared_ptr<Shader> shader)
{
	if (m_SpawnedEntities.size() >= m_EntityLimit) return nullptr;


	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 99);
	int r = dist(rng);
	shared_ptr<GameObject> go;

	if (r < m_BunnyProb && m_SpawnedBunnies.size() < m_BunnyLimit) {
		return SpawnBunny(shader);
	}
	else if (r < (m_BunnyProb + m_BearProb) && m_SpawnedBears.size() < m_BearLimit) {
		return SpawnBear(shader);
	}
	else if (r < (m_BunnyProb + m_BearProb + m_SkunkProb) && m_SpawnedSkunks.size() < m_SkunkLimit) {
		return SpawnSkunk(shader);
	}

}
