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
#include <Engine/Animation/Animation.h>
#include <Engine/Animation/Animator.h>

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

	m_AssetMgr->GetTexture("res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_White.png");
	m_AssetMgr->GetTexture("res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_Black.png");
	m_AssetMgr->GetTexture("res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_Brown.png");
	m_AssetMgr->GetTexture("res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_Grey.png");
	m_AssetMgr->GetTexture("res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor_DarkBrown.png");
	m_AssetMgr->GetTexture("res/models/animals/bear-2/bear_textures_1500/bear_1500_bear1_BaseColor.png");

	m_Binder = EventBinder();
	
	CreateEntities(m_AssetMgr->BasicShader);

	m_SpawnWaitTimer = m_SpawnWait * 0.75f;

	m_Spawners = m_Owner->FindComponentsInChildren<Spawner>();
	for (int i = 0; i < m_Spawners.size(); i++)
	{
		m_Spawners[i]->Initialize(i);
		m_Binder.Bind(m_Spawners[i]->OnAnimalEntered, [this](Animal* animal)
		{
			DespawnAnimalWithReward(animal);
		});
	}
}

void SpawnManager::Reset()
{
	// Reset scores and animation
	m_EarnedMoney = 0;
	m_AnimatedMoney = 0.0f;

	// Reset spawn timer
	m_SpawnWaitTimer = m_SpawnWait * 0.75f;

	// Despawn all currently spawned animals (they will be moved to sleep pool)
	while (!m_SpawnedAnimalsPool.empty())
	{
		auto animal = m_SpawnedAnimalsPool.back();
		DespawnAnimal(animal.get());
	}

	// Clear sleep pool timers
	m_SleepAnimalsPool.clear();

	// Exile all animals in pool
	for (auto &a : m_AnimalsPool)
	{
		if (a != nullptr)
		{
			a->SetActive(false);
			RigidBody* rb = a->GetComponent<RigidBody>();
			if (rb != nullptr)
			{
				rb->RequestTeleport(m_ExiledPos);
			}
		}
	}
}


void SpawnManager::Update()
{
	Behaviour::Update();

	UpdateTimers();

	// 1. Sekcja odpowiedzialna za odliczanie czasu do spawnu zwierząt
	if (m_SpawnedAnimalsPool.size() < m_SpawnedLimit)
	{
		m_SpawnWaitTimer += Time::GetDeltaTime();
		if (m_SpawnWaitTimer >= m_SpawnWait)
		{
			m_SpawnWaitTimer = 0;
			if (m_AnimalsPool.size() > 0)
			{
				shared_ptr<GameObject> animal = PickAnimal();
				int spawnerID = Random::GetRandomInt(0, m_Spawners.size() - 1);
				SetSpawnValue(animal.get(), spawnerID);
				SpawnAnimal(animal.get(), spawnerID);
			}
		}
	}
	
	if (m_AnimatedMoney < m_EarnedMoney)
	{
		m_AnimatedMoney += (m_EarnedMoney - m_AnimatedMoney) * 2.0f * Time::GetDeltaTime();

		if (m_EarnedMoney - m_AnimatedMoney < 0.1f)
		{
			m_AnimatedMoney = static_cast<float>(m_EarnedMoney);
		}
	}

	UpdateSleepingAnimals();

}

void SpawnManager::UpdateSleepingAnimals()
{
	for (int i = 0; i < m_SleepAnimalsPool.size(); i++)
	{
		SleepingAnimal& sleeper = m_SleepAnimalsPool[i];
		sleeper.SleepTimer += Time::GetDeltaTime();
		if (sleeper.SleepTimer >= m_SleepDuration)
		{
			swap(m_SleepAnimalsPool[i], m_SleepAnimalsPool.back());
			m_SleepAnimalsPool.pop_back();
			m_AnimalsPool.push_back(sleeper.Animal);
			i--;
		}
	}
}

void SpawnManager::DecreaseWaitTimer()
{
	m_SpawnWaitTimer += m_WaitTimeDecrease;
}

void SpawnManager::UpdateTimers()
{
	if (m_SceneMgr != nullptr)
	{
		float ratio = m_SceneMgr->GetTimeProgressRatio();
		m_WaitTimeDecrease = lerp(MIN_WAIT_TIME_DECREASE, MAX_WAIT_TIME_DECREASE, ratio);
		m_SpawnWait = lerp(MAX_SPAWN_WAIT, MIN_SPAWN_WAIT, ratio);
	}
}

void SpawnManager::AddMoney(int money)
{
	m_EarnedMoney += money;
}

void SpawnManager::DespawnAnimalWithReward(Animal* animal)
{
	if (animal->GetRequiredServices().empty())
	{
		Engine::GetInstance().GetAudioManager().PlaySound("res/audio/money.wav", 1.f);
		AddMoney(100);
	}
	DespawnAnimal(animal->GetOwner());
}

shared_ptr<GameObject> SpawnManager::PickAnimal()
{
	int index = Random::GetRandomInt(0, m_AnimalsPool.size() - 1);
	if (index >= m_AnimalsPool.size())
	{
		std::cout << "ERROR: Animal spawner wanted bigger index, tweak random calculator\n";
		return nullptr;
	}
			
	shared_ptr<GameObject> animal = m_AnimalsPool[index];
	swap(m_AnimalsPool[index], m_AnimalsPool.back());
	m_AnimalsPool.pop_back();
	m_SpawnedAnimalsPool.push_back(animal);
	animal->SetActive(true);
	return animal;
}

void SpawnManager::SetSpawnValue(GameObject* animal, int spawnerID)
{
	glm::vec3 spawnPosition = m_Spawners[spawnerID]->GetSpawnPosition();
	// spawnPosition.x -= 5;
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

void SpawnManager::SpawnAnimal(GameObject* animal, int spawnerID)
{
	RigidBody* animalRB = animal->GetComponent<RigidBody>();
	glm::vec3 throwVelocity = m_Spawners[spawnerID]->GetSpawnerVelocity() * 40.f;
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
				// m_AnimalsPool.push_back(spawnedAnimal);
				m_SleepAnimalsPool.push_back(SleepingAnimal(spawnedAnimal));
				Animal* anScript = spawnedAnimal->GetComponent<Animal>();
				animal->SetActive(false);
				if (anScript != nullptr)
				{
					anScript->m_IsInitialized = false;
					anScript->GetStateController()->RequestStateChange.Invoke(AnimalState::None);
				}
				RigidBody* rb = spawnedAnimal->GetComponent<RigidBody>();
				if (rb != nullptr)
				{
					rb->RequestTeleport(m_ExiledPos);
				}
				DecreaseWaitTimer();
				return;
			}
		}
	}
}

shared_ptr<GameObject> SpawnManager::CreateAnimal(shared_ptr<Shader> shader, const char* path, const char* name, int index, std::string animalPath)
{
	shared_ptr<GameObject> animal = m_SceneMgr->Instantiate(m_AnimalParent, path, shader);
	animal->Name = name + std::to_string(index);
	// animal->AddComponent<RigidBody>();
	// animal->GetComponent<RigidBody>()->PrepareInit();
	Animal* anScrp = animal->AddComponent<Animal>();
	m_Binder.Bind(anScrp->OnEnteredOnsenObject, [this](AnimalNeeds need) { this->DecreaseWaitTimer(); });

	if (!animalPath.empty())
	{
		Model* model = animal->GetComponent<Model>();
		if (model != nullptr)
		{
			Animation* startAnimation = new Animation(animalPath, model);
			animal->AddComponent<Animator>(startAnimation);
		}
	}

	animal->transform->Position = m_ExiledPos;
	animal->UpdateSelfAndChild();
	//animal->AddComponent<TutorialAnimal>();
	m_SceneMgr->AddAnimal(animal);
	animal->SetActive(false);
	return animal;
}

shared_ptr<GameObject> SpawnManager::CreateBunny(shared_ptr<Shader> shader, std::shared_ptr<Shader> animShader, int index)
{
	shared_ptr<GameObject> animal = CreateAnimal(animShader, "res/models/animations/animals/bunny-animations/bunny-waiting.glb", "bunny", index, "res/models/animations/animals/bunny-animations/bunny-waiting.glb");
	Animator* animator = animal->GetComponent<Animator>();
	Model* model = animal->GetComponent<Model>();

	if (animator != nullptr && model != nullptr)
	{
		animator->AddAnimation("idle", new Animation("res/models/animations/animals/bunny-animations/bunny-waiting.glb", model));
		animator->AddAnimation("walk", new Animation("res/models/animations/animals/bunny-animations/bunny-walking.glb", model));
		animator->AddAnimation("eaten", new Animation("res/models/animations/animals/bunny-animations/bunny-fear.glb", model));
		animator->AddAnimation("run", new Animation("res/models/animations/animals/bunny-animations/bunny-running-away.glb", model));
		animator->PlayAnimation("idle");
	}
	return animal;
}

shared_ptr<GameObject> SpawnManager::CreateBear(shared_ptr<Shader> shader, std::shared_ptr<Shader> animShader, int index)
{
	shared_ptr<GameObject> animal = CreateAnimal(animShader, "res/models/animations/animals/bear-animations/bear-waiting.glb", "bear", index, "res/models/animations/animals/bear-animations/bear-waiting.glb");
	Animator* animator = animal->GetComponent<Animator>();
	Model* model = animal->GetComponent<Model>();

	if (animator != nullptr && model != nullptr)
	{
		animator->AddAnimation("idle", new Animation("res/models/animations/animals/bear-animations/bear-waiting.glb", model));
		animator->AddAnimation("walk", new Animation("res/models/animations/animals/bear-animations/bear-walking.glb", model));
		animator->AddAnimation("eat", new Animation("res/models/animations/animals/bear-animations/bear-eating.glb", model));
		animator->AddAnimation("run", new Animation("res/models/animations/animals/bear-animations/bear-running.glb", model));
		animator->PlayAnimation("idle");
	}
	return animal;
}

shared_ptr<GameObject> SpawnManager::CreateSkunk(shared_ptr<Shader> shader, std::shared_ptr<Shader> animShader, int index)
{
	shared_ptr<GameObject> animal = CreateAnimal(animShader, "res/models/animations/animals/skunks-animations/skunks-waiting.glb", "skunk", index, "res/models/animations/animals/skunks-animations/skunks-waiting.glb");
	Animator* animator = animal->GetComponent<Animator>();
	Model* model = animal->GetComponent<Model>();

	if (animator != nullptr && model != nullptr)
	{
		animator->AddAnimation("idle", new Animation("res/models/animations/animals/skunks-animations/skunks-waiting.glb", model));
		animator->AddAnimation("walk", new Animation("res/models/animations/animals/skunks-animations/skunks-walking.glb", model));
		animator->PlayAnimation("idle");
	}
	return animal;
}

void SpawnManager::CreateEntities(shared_ptr<Shader> shader)
{
	int r = Random::GetRandomInt(0, 99);
	shared_ptr<Shader> animatedShader = m_AssetMgr->AnimatedShader;

	for (int i = 0; i < m_BunnyLimit; i++)
	{
		m_AnimalsPool.push_back(CreateBunny(shader, animatedShader, i + 1));
	}
	for (int i = 0; i < m_BearLimit; i++)
	{
		m_AnimalsPool.push_back(CreateBear(shader, animatedShader, i + 1));
	}
	for (int i = 0; i < m_SkunkLimit; i++)
	{
		m_AnimalsPool.push_back(CreateSkunk(shader, animatedShader, i + 1));
	}
}

