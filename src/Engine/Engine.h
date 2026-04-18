#pragma once
#include <memory>

#include "WindowManager.h"
#include "DebugManager.h"

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
	
	std::unique_ptr<WindowManager> m_WindowMgr;
	std::unique_ptr<DebugManager> m_DebugMgr;
	
	void Initialize();
public:
	const char* GLSL_VERSION = "#version 410";
	const int32_t GL_VERSION_MAJOR = 4;
	const int32_t GL_VERSION_MINOR = 1;

	bool GetIsDebugDrawn();
	static Engine& GetInstance();
	WindowManager& GetWindowManager();
	DebugManager& GetDebugManager();
	void Start();
};
