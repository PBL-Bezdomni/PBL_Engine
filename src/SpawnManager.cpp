#include "SpawnManager.h"
#include "Engine/Loader.h"
#include <random>
// #include "Model.h"
#include "GameObject.h"
#include "Shader.h"

SpawnManager::SpawnManager()
{
	
}


GameObject* SpawnManager::SpawnBall(Shader& shader)
{
	if (m_SpawnedBalls.size() < m_BallLimit)
	{
		auto ball = make_unique<GameObject>(); //Entity(shader, (loader.RelativePath + "res/models/sphere/sphere.obj").c_str());
		ball->AddComponent<Model>(shader, (Loader::RelativePath() + "res/models/sphere/sphere.obj").c_str());
		GameObject* ptr = ball.get();
		m_SpawnedBalls.push_back(move(ball));
		m_SpawnedEntities.push_back(move(ball));
		return ptr;
	}
	return nullptr;
}

GameObject* SpawnManager::SpawnMonkey(Shader& shader)
{
	if (m_SpawnedMonkeys.size() < m_MonkeyLimit)
	{
		auto monkey = make_unique<GameObject>();
		monkey->AddComponent<Model>(shader, (Loader::RelativePath() + "res/models/monkey/Monkey.obj").c_str());
		GameObject* ptr = monkey.get();
		m_SpawnedBalls.push_back(move(monkey));
		m_SpawnedEntities.push_back(move(monkey));
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
			return SpawnMonkey(shader);
		}
		else
		{
			return SpawnBall(shader);
		}
	}
	return nullptr;
}


