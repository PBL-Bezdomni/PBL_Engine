#pragma once
#include <memory>
#include "SceneManager.h"

// class PhysicsEngine;
// class WindowManager;

class GameManager
{
private:
	std::unique_ptr<SceneManager> m_SceneMgr;

	void GameLoop();
	void LoadScene();
public:
	GameManager() = default;
	void Initialize();
	void StartGame();

private:
	//TODO Remove or move elsewhere

	const float FIXED_TIME_STEP = 1.0f / 60.0f;
	float physicsAccumulator = 0.0f;
	
	PhysicsEngine* Physics = nullptr;
	WindowManager* WindowMgr = nullptr;
};
