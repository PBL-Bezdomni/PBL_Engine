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
	
	m_WindowMgr = make_unique<WindowManager>();
	m_WindowMgr->Initialize(GL_VERSION_MAJOR, GL_VERSION_MINOR);
	
	m_PhysicsEngine = make_unique<PhysicsEngine>();
	m_PhysicsEngine->Init();

	m_DebugMgr = make_unique<DebugManager>();
	m_DebugMgr->InitializeImGUI(m_WindowMgr->GetWindowPointer(), GLSL_VERSION);
	
	m_AssetMgr = make_unique<AssetManager>();
	m_AssetMgr->Initialize();
}

bool Engine::GetIsDebugDrawn()
{
	return m_IsDebugDraw;
}


void Engine::Start()
{
	// TODO start engine
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

