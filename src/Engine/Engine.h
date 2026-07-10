#pragma once
#include <memory>

#include "AssetManager.h"
#include "AudioManager.h"
#include "WindowManager.h"
#include "DebugManager.h"
#include "Game/GameManager.h"
#include "PhysicsEngine/PhysicsEngine.h"

class Engine
{
private:
	// DEBUG
	bool m_IsDebugDraw = false;
	int m_DrawCounter = 0;
	int m_InstanceDrawCounter = 0;
	std::vector<int> m_DrawPerFrameCol;
	std::vector<int> m_InstanceDrawPerFrameCol;
	std::vector<int> m_AllDrawPerFrameCol;
	std::vector<int> m_FPSCol;
	std::vector<float> m_FrameDurationCol;
	int m_CollectionIndex;
	int COLLECTION_AVERAGE = 10;
	
	const float FIXED_TIME_STEP = 1.0f / 60.0f;
	
	bool m_IsInitialized = false;
	float m_PhysicsAccumulator = 0.0f;
	
	Engine() = default;
	~Engine() = default;
	
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	std::unique_ptr<PhysicsEngine> m_PhysicsEngine;
	std::unique_ptr<WindowManager> m_WindowMgr;
	std::unique_ptr<DebugManager> m_DebugMgr;
	std::unique_ptr<AssetManager> m_AssetMgr;
	std::unique_ptr<AudioManager> m_AudioMgr;
	std::unique_ptr<GameManager> m_GameMgr;
	
	void Initialize();
	void SecondPassInitialization();
public:
	const char* GLSL_VERSION = "#version 430";
	const int32_t GL_VERSION_MAJOR = 4;
	const int32_t GL_VERSION_MINOR = 3;

	bool GetIsDebugDrawn();
	void ToggleDebugDraw();
	static Engine& GetInstance();
	PhysicsEngine& GetPhysicsEngine();
	WindowManager& GetWindowManager();
	AssetManager& GetAssetManager();
	AudioManager& GetAudioManager();
	GameManager& GetGameManager();
	void Start();
	void Stop();
	int MainLoop();
	
	void IncreaseDrawCount(bool isInstance);
	void FrameEnded(int fps, float dt);
};
