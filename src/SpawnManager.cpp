#include "SpawnManager.h"
#include "Engine/Loader.h"
#include <random>
// #include "Model.h"
#include "GameObject.h"
#include "Shader.h"
#include "Engine/Engine.h"

void SpawnManager::Initialize()
{
	m_AssetMgr = &Engine::GetInstance().GetAssetManager();
}


GameObject* SpawnManager::SpawnBunny(Shader& shader)
{
	if (m_SpawnedBalls.size() < m_BallLimit)
	{
		auto bunny = make_unique<GameObject>();
		Model bunnyModel = *m_AssetMgr->GetModel(shader, "res/models/animals/bunny/bunny.fbx");
		bunny->AddComponent<Model>(bunnyModel);
		GameObject* ptr = bunny.get();
		m_SpawnedBalls.push_back(move(bunny));
		m_SpawnedEntities.push_back(move(bunny));
		return ptr;
	}
	return nullptr;
}

GameObject* SpawnManager::SpawnBear(Shader& shader)
{
	if (m_SpawnedMonkeys.size() < m_MonkeyLimit)
	{
		auto bear = make_unique<GameObject>();
		Model bearModel = *m_AssetMgr->GetModel(shader, "res/models/animals/bear/bear_1500.fbx");
		bear->AddComponent<Model>(bearModel);
		GameObject* ptr = bear.get();
		m_SpawnedBalls.push_back(move(bear));
		m_SpawnedEntities.push_back(move(bear));
		return ptr;
	}
	return nullptr;
}

GameObject* SpawnManager::SpawnEntity(Shader& shader)
{
	if (m_SpawnedEntities.size() < m_MainLimit)
	{
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0,99);
		int r = dist(rng);
		if (r < m_MonkeyProb && m_SpawnedMonkeys.size() < m_MonkeyLimit)
		{
			return SpawnBear(shader);
		}
		else
		{
			return SpawnBunny(shader);
		}
	}
	return nullptr;
}


