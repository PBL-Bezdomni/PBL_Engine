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
	float WALL_Y_BORDER = 50.f;
	
	float m_SpawnCounter;
	float m_SpawnTime = 2.f;
	
	int m_EntityLimit = 10;

	int m_BunnyLimit = 15;
	int m_BearLimit = 5;
	int m_SkunkLimit = 7;

	vector<shared_ptr<GameObject>> m_SpawnedEntities;
	vector<shared_ptr<GameObject>> m_SpawnedBunnies;
	vector<shared_ptr<GameObject>> m_SpawnedBears;
	vector<shared_ptr<GameObject>> m_SpawnedSkunks;

	float m_BunnyProb = 50.f;	
	float m_BearProb = 15.f;
	float m_SkunkProb = 35.f;

	AssetManager* m_AssetMgr;
	SceneManager* m_SceneMgr;
public:
	SpawnManager() = default;
	shared_ptr<GameObject> SpawnEntity(shared_ptr<Shader> shader);
	shared_ptr<GameObject> SpawnBunny(shared_ptr<Shader> shader);
	shared_ptr<GameObject> SpawnBear(shared_ptr<Shader> shader);
	shared_ptr<GameObject> SpawnSkunk(shared_ptr<Shader> shader);


	void Awake() override;
	void Start() override;
	void Update() override;
};
