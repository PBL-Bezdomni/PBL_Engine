#pragma once
#include <list>
#include <memory>
#include "Engine/AssetManager.h"
#include "Engine/Loader.h"
#include "Engine/Components/Behaviour.h"

class GameObject;
class Shader;
class SceneManager;

class SpawnManager : public Behaviour
{
private:
	float WALL_X_BORDER = 10.f;
	float WALL_Y_BORDER = 30.f;
	
	float m_SpawnCounter;
	float m_SpawnTime = 2.f;

	int m_BunnyLimit = 2; //7
	int m_BearLimit = 1; //3
	int m_SkunkLimit = 2; //4
	int m_SpawnedLimit = 5;
	int m_EarnedMoney = 0;
	

	GameObject* m_AnimalParent;
	AssetManager* m_AssetMgr;
	SceneManager* m_SceneMgr;
	vector<shared_ptr<GameObject>> m_AnimalsPool;
	vector<shared_ptr<GameObject>> m_SpawnedAnimalsPool;

	glm::vec3 m_ExiledPos = glm::vec3(1000, 5, -1000);
public:
	static SpawnManager* Instance;

	void AddMoney(int money);
	int GetMoney() const { return m_EarnedMoney; }
	SpawnManager() = default;
	void CreateEntities(shared_ptr<Shader> shader);
	shared_ptr<GameObject> CreateAnimal(shared_ptr<Shader> shader, const char* path, const char* name, int index = 0, std::string animPat = "");
	shared_ptr<GameObject> CreateBunny(shared_ptr<Shader> shader, int index = 0);
	shared_ptr<GameObject> CreateBear(shared_ptr<Shader> shader, std::shared_ptr<Shader> animShader, int index = 0);
	shared_ptr<GameObject> CreateSkunk(shared_ptr<Shader> shader, int index = 0);

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
