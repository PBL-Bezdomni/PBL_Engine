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

	// Start background music if available
	// Note: file extension must match an actual file in res/audio, e.g. runAmok.mp3 or runAmok.wav
	// m_AudioMgr->PlayLoop("res/audio/runAmok.mp3");

	m_GameMgr = std::make_unique<GameManager>();
	// m_GameMgr->Initialize();

	m_DrawPerFrameCol.resize(COLLECTION_AVERAGE);
	m_InstanceDrawPerFrameCol.resize(COLLECTION_AVERAGE);
	m_AllDrawPerFrameCol.resize(COLLECTION_AVERAGE);
	m_FPSCol.resize(COLLECTION_AVERAGE);
	m_FrameDurationCol.resize(COLLECTION_AVERAGE);
}

void Engine::SecondPassInitialization()
{
	m_GameMgr->Initialize();
	m_DebugMgr->InitializeImGUI(m_WindowMgr->GetWindowPointer(), GLSL_VERSION);
}

bool Engine::GetIsDebugDrawn()
{
	return m_IsDebugDraw;
}

void Engine::ToggleDebugDraw()
{
	m_IsDebugDraw = !m_IsDebugDraw;
}

PhysicsEngine& Engine::GetPhysicsEngine()
{
	return *m_PhysicsEngine;
}

WindowManager& Engine::GetWindowManager()
{
	return *m_WindowMgr;
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

void Engine::IncreaseDrawCount(bool isInstance)
{
	if (isInstance)
	{
		m_InstanceDrawCounter++;
	}
	else
	{
		m_DrawCounter++;
	}
}

void Engine::FrameEnded(int fps, float dt)
{
	m_CollectionIndex++;
	m_CollectionIndex %= COLLECTION_AVERAGE;

	m_DrawPerFrameCol[m_CollectionIndex] = m_DrawCounter;
	m_InstanceDrawPerFrameCol[m_CollectionIndex] = m_InstanceDrawCounter; 
	m_AllDrawPerFrameCol[m_CollectionIndex] = m_DrawCounter + m_InstanceDrawCounter;
	m_DrawCounter = 0;
	m_InstanceDrawCounter = 0;
	m_FPSCol[m_CollectionIndex] = fps;
	m_FrameDurationCol[m_CollectionIndex] = dt; 
}

void Engine::Stop()
{
	int avDraw = 0;
	int avInsDraw = 0;
	int avAllDraw = 0;
	int avFPS = 0;
	float avDT = 0;
	
	for (int i = 0; i < COLLECTION_AVERAGE; i++)
	{
		avDraw += m_DrawPerFrameCol[i];
		avInsDraw += m_InstanceDrawPerFrameCol[i];
		avAllDraw += m_AllDrawPerFrameCol[i];
		avFPS += m_FPSCol[i];
		avDT += m_FrameDurationCol[i];
	}
	avDraw = avDraw / COLLECTION_AVERAGE;
	avInsDraw = avInsDraw / COLLECTION_AVERAGE;
	avAllDraw = avAllDraw / COLLECTION_AVERAGE;
	avFPS /= COLLECTION_AVERAGE;
	avDT /= COLLECTION_AVERAGE;
	avDT *= 1000;

	JSONImporter js = JSONImporter();
	js.SaveOptimizationStats(avDraw, avInsDraw, avAllDraw, avFPS, avDT);
}

