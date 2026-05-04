#include "GameManager.h"
#include "Engine/WindowManager.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include "Engine/Time.h"
#include "Engine/Engine.h"

void GameManager::Initialize()
{
	m_SceneMgr = std::make_unique<SceneManager>();
}

void GameManager::StartGame()
{
	Engine& engine = Engine::GetInstance();
	Physics = &engine.GetPhysicsEngine();
	WindowMgr = &engine.GetWindowManager();
	
	LoadScene();
}

void GameManager::UpdateGame()
{
	// Process I/O operations here
	m_SceneMgr->UpdateScene();
}
void GameManager::RenderGame()
{
	// OpenGL rendering code here
	m_SceneMgr->RenderScene();
}

void GameManager::LoadScene()
{
	m_SceneMgr->Initialize();
}

