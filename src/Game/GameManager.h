#pragma once
#include <memory>
#include "SceneManager.h"

// class PhysicsEngine;
// class WindowManager;

class GameManager
{
private:
	std::unique_ptr<SceneManager> m_SceneMgr;
	
	void LoadScene();
public:
	GameManager() = default;
	void Initialize();
	void StartGame();
	void UpdateGame();
	void RenderGame();
private:	
	PhysicsEngine* Physics = nullptr;
	WindowManager* WindowMgr = nullptr;
};
