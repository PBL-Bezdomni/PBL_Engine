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
	float WALL_Y_BORDER = 10.f;
	
	float m_SpawnCounter;
	float m_SpawnTime = 2.f;
	
	int m_MainLimit = 10;
	int m_MonkeyLimit = 3;
	int m_BallLimit = 13;
	vector<shared_ptr<GameObject>> m_SpawnedEntities;
	vector<shared_ptr<GameObject>> m_SpawnedBalls;
	vector<shared_ptr<GameObject>> m_SpawnedMonkeys;

	float m_MonkeyProb = 20.f;	
	float m_BallProb = 70.f;
	AssetManager* m_AssetMgr;
	SceneManager* m_SceneMgr;
public:
	SpawnManager() = default;
	shared_ptr<GameObject> SpawnBunny(shared_ptr<Shader> shader);
	shared_ptr<GameObject> SpawnBear(shared_ptr<Shader> shader);
	shared_ptr<GameObject> SpawnEntity(shared_ptr<Shader> shader);

	void Awake() override;
	void Start() override;
	void Update() override;
};
