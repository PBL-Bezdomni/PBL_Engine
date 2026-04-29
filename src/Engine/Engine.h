#pragma once
#include <memory>

#include "AssetManager.h"
#include "WindowManager.h"
#include "DebugManager.h"
#include "Game/GameManager.h"
#include "PhysicsEngine/PhysicsEngine.h"

class Engine
{
private:
	// DEBUG
	bool m_IsDebugDraw = false;
	bool m_IsInitialized = false;
	Engine() = default;
	~Engine() = default;
	
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	std::unique_ptr<PhysicsEngine> m_PhysicsEngine;
	std::unique_ptr<WindowManager> m_WindowMgr;
	std::unique_ptr<DebugManager> m_DebugMgr;
	std::unique_ptr<AssetManager> m_AssetMgr;
	std::unique_ptr<GameManager> m_GameMgr;
	
	void Initialize();
public:
	const char* GLSL_VERSION = "#version 410";
	const int32_t GL_VERSION_MAJOR = 4;
	const int32_t GL_VERSION_MINOR = 1;

	bool GetIsDebugDrawn();
	static Engine& GetInstance();
	PhysicsEngine& GetPhysicsEngine();
	WindowManager& GetWindowManager();
	DebugManager& GetDebugManager();
	AssetManager& GetAssetManager();
	GameManager& GetGameManager();
	void Start();
};
