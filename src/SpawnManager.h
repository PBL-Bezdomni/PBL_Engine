#pragma once
#include <list>
#include <memory>


#include "Engine/Loader.h"

class GameObject;
class Shader;

class SpawnManager
{
private:
	int m_MainLimit = 10;
	int m_MonkeyLimit = 3;
	int m_BallLimit = 13;
	list<unique_ptr<GameObject>> m_SpawnedEntities;
	list<unique_ptr<GameObject>> m_SpawnedBalls;
	list<unique_ptr<GameObject>> m_SpawnedMonkeys;

	float m_MonkeyProb = 20.f;	
	float m_BallProb = 70.f;
public:
	SpawnManager();
	GameObject* SpawnBall(Shader& shader);
	GameObject* SpawnMonkey(Shader& shader);
	GameObject* SpawnEntity(Shader& shader);
};
