#include "Engine.h"
#include "Loader.h"

Engine& Engine::GetInstance()
{
	static Engine instance;
	if (!instance.m_IsInitialized)
	{
		instance.Initialize();
		instance.m_IsInitialized = true;
	}
	return instance;
}

void Engine::Initialize()
{
	Loader::Initialize();
	
	m_WindowMgr = std::make_unique<WindowManager>();
	m_WindowMgr->Initialize(GL_VERSION_MAJOR, GL_VERSION_MINOR);
	
	m_PhysicsEngine = std::make_unique<PhysicsEngine>();
	m_PhysicsEngine->Init();

	m_DebugMgr = std::make_unique<DebugManager>();
	m_DebugMgr->InitializeImGUI(m_WindowMgr->GetWindowPointer(), GLSL_VERSION);
	
	m_AssetMgr = std::make_unique<AssetManager>();
	m_AssetMgr->Initialize();

	m_GameMgr = std::make_unique<GameManager>();
	m_GameMgr->Initialize();
}

bool Engine::GetIsDebugDrawn()
{
	return m_IsDebugDraw;
}


void Engine::Start()
{
	if (m_GameMgr != nullptr)
	{
		m_GameMgr->StartGame();
	}
}

PhysicsEngine& Engine::GetPhysicsEngine()
{
	return *m_PhysicsEngine;
}

WindowManager& Engine::GetWindowManager()
{
	return *m_WindowMgr;
}

DebugManager& Engine::GetDebugManager()
{
	return *m_DebugMgr;
}

AssetManager& Engine::GetAssetManager()
{
	return *m_AssetMgr;
}

GameManager& Engine::GetGameManager()
{
	return *m_GameMgr;
}
