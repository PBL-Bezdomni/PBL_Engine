#pragma once
#include <list>
#include <memory>

#include "Entity.h"
#include "Engine/Loader.h"

class SpawnManager
{
private:
	int m_MainLimit = 10;
	int m_MonkeyLimit = 3;
	int m_BallLimit = 13;
	list<unique_ptr<Entity>> m_SpawnedEntities;
	list<unique_ptr<Entity>> m_SpawnedBalls;
	list<unique_ptr<Entity>> m_SpawnedMonkeys;

	float m_MonkeyProb = 20.f;	
	float m_BallProb = 70.f;
public:
	SpawnManager();
	Entity* SpawnBall(Shader& shader);
	Entity* SpawnMonkey(Shader& shader);
	Entity* SpawnEntity(Shader& shader);
};
