#pragma once
#include <list>
#include <memory>
#include "Engine/AssetManager.h"
#include "Engine/Loader.h"
#include "Engine/Components/Behaviour.h"
#include "Engine/Events/EventBinder.h"

enum class AnimalNeeds;
class GameObject;
class Shader;
class SceneManager;

struct SleepingAnimal
{
	shared_ptr<GameObject> Animal;
	float SleepTimer;

	SleepingAnimal(shared_ptr<GameObject> animal)
	{
		Animal = animal;
		SleepTimer = 0;
	}
};

class SpawnManager : public Behaviour
{
private:
	const  float MAX_WAIT_TIME_DECREASE = 12;
	const  float MIN_WAIT_TIME_DECREASE = 3;
	const float MAX_SPAWN_WAIT = 20.f;
	const float MIN_SPAWN_WAIT = 7.f;
	
	float WALL_X_BORDER = 10.f;
	float WALL_Y_BORDER = 30.f;
	
	float m_SpawnWaitTimer;
	float m_AnimatedMoney = 0.0f;

	int m_BunnyLimit = 6;
	int m_BearLimit = 2;
	int m_SkunkLimit = 3;
	int m_SpawnedLimit = 5;
	int m_EarnedMoney = 0;
	float m_SleepDuration = 5.f;
	float m_SpawnWait = MAX_SPAWN_WAIT;
	float m_WaitTimeDecrease = MIN_WAIT_TIME_DECREASE;

	EventBinder m_Binder;

	GameObject* m_AnimalParent;
	AssetManager* m_AssetMgr;
	SceneManager* m_SceneMgr;
	vector<shared_ptr<GameObject>> m_AnimalsPool;
	vector<shared_ptr<GameObject>> m_SpawnedAnimalsPool;
	vector<SleepingAnimal> m_SleepAnimalsPool;

	glm::vec3 m_ExiledPos = glm::vec3(1000, 5, -1000);

	void UpdateSleepingAnimals();
	void DecreaseWaitTimer();
	void UpdateTimers();
public:
	static SpawnManager* Instance;

	void AddMoney(int money);
	int GetMoney() const { return m_EarnedMoney; }
	int GetMoneyAnimated() const { return static_cast<int>(m_AnimatedMoney); }
	bool IsMoneyAnimating() const { return m_AnimatedMoney < m_EarnedMoney; }
	SpawnManager() = default;
	void CreateEntities(shared_ptr<Shader> shader);
	shared_ptr<GameObject> CreateAnimal(shared_ptr<Shader> shader, const char* path, const char* name, int index = 0, std::string animPat = "");
	shared_ptr<GameObject> CreateBunny(shared_ptr<Shader> shader, std::shared_ptr<Shader> animShader, int index = 0);
	shared_ptr<GameObject> CreateBear(shared_ptr<Shader> shader, std::shared_ptr<Shader> animShader, int index = 0);
	shared_ptr<GameObject> CreateSkunk(shared_ptr<Shader> shader, std::shared_ptr<Shader> animShader, int index = 0);

	shared_ptr<GameObject> PickAnimal();
	void SetSpawnValue(GameObject* animal);
	void SpawnAnimal(GameObject* animal);
	void DespawnAnimal(GameObject* animal);
	
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnTriggerEnter(GameObject* other) override;

	const char* GetScriptName() const override { return "SpawnManager"; }
};
