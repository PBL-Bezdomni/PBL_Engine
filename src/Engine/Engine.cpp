#include "Engine.h"
#include "Loader.h"
#include "Time.h"

Engine& Engine::GetInstance()
{
	static Engine instance;
	if (!instance.m_IsInitialized)
	{
		instance.Initialize();
		instance.m_IsInitialized = true;
		instance.SecondPassInitialization();
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
	// m_DebugMgr->InitializeImGUI(m_WindowMgr->GetWindowPointer(), GLSL_VERSION);
	
	m_AssetMgr = std::make_unique<AssetManager>();
	m_AssetMgr->Initialize();

	m_AudioMgr = std::make_unique<AudioManager>();
	m_AudioMgr->Initialize();

	m_GameMgr = std::make_unique<GameManager>();
	m_GameMgr->Initialize();
}

void Engine::SecondPassInitialization()
{
	// m_GameMgr->LoadScene();
	m_DebugMgr->InitializeImGUI(m_WindowMgr->GetWindowPointer(), GLSL_VERSION);
}

bool Engine::GetIsDebugDrawn()
{
	return m_IsDebugDraw;
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

AudioManager& Engine::GetAudioManager()
{
	return *m_AudioMgr;
}

GameManager& Engine::GetGameManager()
{
	return *m_GameMgr;
}

void Engine::Start()
{
	if (m_GameMgr != nullptr)
	{
		m_GameMgr->StartGame();
		MainLoop();
	}
}

int Engine::MainLoop()
{
	while (!m_WindowMgr->ShouldWindowClose())
	{
		// Clear screen
		m_WindowMgr->ClearFrame();

		// Process I/O operations here
		m_GameMgr->UpdateGame();
		
		// Update game objects' state here
		Time::Update();

		float frameTime = Time::GetDeltaTime();
		if (frameTime > 0.25f) {
			frameTime = 0.25f;
		}

		m_PhysicsAccumulator += frameTime;

		while (m_PhysicsAccumulator >= FIXED_TIME_STEP)
		{
			m_PhysicsEngine->Update(FIXED_TIME_STEP);
			m_PhysicsAccumulator -= FIXED_TIME_STEP;
		}
		
		// OpenGL rendering code here
		m_GameMgr->RenderGame();

		if (m_IsDebugDraw)
		{
			m_DebugMgr->RenderImgui(m_WindowMgr->GetWindowPointer());
		}

		// End frame and swap buffers (double buffering)
		m_WindowMgr->EndFrame();
	}

	// TODO stop everything
	return 0;
}

