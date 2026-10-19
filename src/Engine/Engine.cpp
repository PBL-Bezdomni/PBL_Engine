#include "Engine.h"
#include "Loader.h"
#include "Time.h"
#include <gl/GL.h>
#include <windows.h>

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

void Engine::FrameEnded(int fps, float dt)
{
	m_CollectionIndex++;
	m_CollectionIndex %= COLLECTION_AVERAGE;
	
	m_FPSCol[m_CollectionIndex] = fps;
	m_FrameDurationCol[m_CollectionIndex] = dt; 
}

void Engine::Stop()
{
	int avFPS = 0;
	float avDT = 0;
	
	for (int i = 0; i < COLLECTION_AVERAGE; i++)
	{
		avFPS += m_FPSCol[i];
		avDT += m_FrameDurationCol[i];
	}
	avFPS /= COLLECTION_AVERAGE;
	avDT /= COLLECTION_AVERAGE;

	SYSTEM_INFO sysInfo = {};
	GetNativeSystemInfo(&sysInfo);

	string cpu = GetCPUInfo();
	string lp = to_string(sysInfo.dwNumberOfProcessors);
	string gpuVendor = GetGPUInfo(0);
	string gpuRenderer = GetGPUInfo(1);
	string glVersion = GetGPUInfo(2);
	string ramTotal = GetRAMInfo(0);
	string ramAvailable = GetRAMInfo(1);

	JSONImporter js = JSONImporter();
	js.SaveLog(cpu, lp, gpuVendor, gpuRenderer, glVersion, ramTotal, ramAvailable, avFPS, avDT);
}

std::string Engine::GetCPUInfo()
{
	HKEY hKey;

	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);
	string cpuName = "Unknown";

	if (result == ERROR_SUCCESS)
	{
		char buffer[512] = {};
		DWORD bufferSize = sizeof(buffer);
		DWORD type = 0;

		result = RegQueryValueExA(hKey, "ProcessorNameString", nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &bufferSize);

		if (result == ERROR_SUCCESS && type == REG_SZ)
		{
			cpuName = buffer;
		}

		RegCloseKey(hKey);
	}

	return cpuName;
}

std::string Engine::GetGPUInfo(int t)
{
	const GLubyte* name;
	if (t == 1)
	{
		name = glGetString(GL_RENDERER);
	}
	else if (t == 2)
	{
		name = glGetString(GL_VERSION);
	}
	else
	{
		name = glGetString(GL_VENDOR);
	}
	
	return  name ? reinterpret_cast<const char*>(name) : "Unknown";
}

std::string Engine::GetRAMInfo(int t)
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);
	const double GB = 1024.0 * 1024.0 * 1024.0;

	unsigned long long ram;
	
	if (t == 0)
	{
		ram = statex.ullTotalPhys;		
	}
	else
	{
		ram = statex.ullAvailPhys;
	}
	
	return to_string(ram / GB);
}

