#include "SpawnManager.h"
#include "Engine/Loader.h"
#include <random>

SpawnManager::SpawnManager()
{
	
}


Entity* SpawnManager::SpawnBall(Shader& shader, Loader loader)
{
	if (m_SpawnedBalls.size() < m_BallLimit)
	{
		auto ball = make_unique<Entity>(shader, (loader.RelativePath + "res/models/sphere/sphere.obj").c_str()); //Entity(shader, (loader.RelativePath + "res/models/sphere/sphere.obj").c_str());
		Entity* ptr = ball.get();
		m_SpawnedBalls.push_back(move(ball));
		m_SpawnedEntities.push_back(move(ball));
		return ptr;
	}
	return nullptr;
}

Entity* SpawnManager::SpawnMonkey(Shader& shader, Loader loader)
{
	if (m_SpawnedMonkeys.size() < m_MonkeyLimit)
	{
		auto monkey = make_unique<Entity>(shader, (loader.RelativePath + "res/models/monkey/Monkey.obj").c_str());
		Entity* ptr = monkey.get();
		m_SpawnedBalls.push_back(move(monkey));
		m_SpawnedEntities.push_back(move(monkey));
		return ptr;
	}
	return nullptr;
}

Entity* SpawnManager::SpawnEntity(Shader& shader, Loader loader)
{
	if (m_SpawnedEntities.size() < m_MainLimit)
	{
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0,99);
		int r = dist(rng);
		if (r < m_MonkeyProb && m_SpawnedMonkeys.size() < m_MonkeyLimit)
		{
			return SpawnMonkey(shader, loader);
		}
		else
		{
			return SpawnBall(shader, loader);
		}
	}
	return nullptr;
}


