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

	int m_BunnyLimit = 7;
	int m_BearLimit = 3;
	int m_SkunkLimit = 4;

	GameObject* m_AnimalParent;
	AssetManager* m_AssetMgr;
	SceneManager* m_SceneMgr;
	vector<shared_ptr<GameObject>> m_AnimalsPool;
public:
	SpawnManager() = default;
	void SpawnEntities(shared_ptr<Shader> shader);
	shared_ptr<GameObject> SpawnAnimal(shared_ptr<Shader> shader, const char* path, const char* name, int index = 0);
	shared_ptr<GameObject> SpawnBunny(shared_ptr<Shader> shader, int index = 0);
	shared_ptr<GameObject> SpawnBear(shared_ptr<Shader> shader, int index = 0);
	shared_ptr<GameObject> SpawnSkunk(shared_ptr<Shader> shader, int index = 0);
	

	void Awake() override;
	void Start() override;
	void Update() override;
};
