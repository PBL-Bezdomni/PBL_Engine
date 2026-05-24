#include "GameManager.h"
#include "Engine/WindowManager.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include "Engine/Time.h"
#include "Engine/Engine.h"

void GameManager::Initialize()
{
	m_InputMgr = std::make_unique<InputManager>();
	m_SceneMgr = std::make_unique<SceneManager>();
	m_SceneMgr->Initialize();
}

InputManager& GameManager::GetInputManager()
{
	return *m_InputMgr;
}

SceneManager& GameManager::GetSceneManager()
{
	return *m_SceneMgr;
}

void GameManager::StartGame()
{
	Engine& engine = Engine::GetInstance();
	Physics = &engine.GetPhysicsEngine();
	WindowMgr = &engine.GetWindowManager();

	// TODO Separate load and initialization of Scene Manager
	LoadScene();
}

void GameManager::UpdateGame()
{
	// Process I/O operations here
	m_InputMgr->update();
	m_SceneMgr->UpdateScene();
}
void GameManager::RenderGame()
{
	// OpenGL rendering code here
	m_SceneMgr->RenderScene();
}

void GameManager::LoadScene()
{
	m_SceneMgr->LoadScene();
}

