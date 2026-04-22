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

	m_DebugMgr = std::make_unique<DebugManager>();
	m_DebugMgr->InitializeImGUI(m_WindowMgr->GetWindowPointer(), GLSL_VERSION);
}

bool Engine::GetIsDebugDrawn()
{
	return m_IsDebugDraw;
}


void Engine::Start()
{
	// TODO start engine
}

WindowManager& Engine::GetWindowManager()
{
	return *m_WindowMgr;
}

DebugManager& Engine::GetDebugManager()
{
	return *m_DebugMgr;
}

