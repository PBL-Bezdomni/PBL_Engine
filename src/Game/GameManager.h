#pragma once
#include <memory>
#include "SceneManager.h"

// class PhysicsEngine;
// class WindowManager;

class GameManager
{
private:
	std::unique_ptr<InputManager> m_InputMgr;
	std::unique_ptr<SceneManager> m_SceneMgr;

	PhysicsEngine* Physics = nullptr;
	WindowManager* WindowMgr = nullptr;
	
public:
	GameManager() = default;
	void Initialize();
	void StartGame();
	void UpdateGame();
	void RenderGame();
	void LoadScene();

	InputManager& GetInputManager();
	SceneManager& GetSceneManager();
};
