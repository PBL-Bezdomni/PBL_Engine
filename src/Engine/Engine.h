#pragma once
#include <glm/glm.hpp>

#include "Engine/WindowManager.h"

class Engine
{
private:
	bool m_IsInitialized = false;
	Engine() = default;
	~Engine() = default;
	
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	
	std::unique_ptr<WindowManager> m_WindowMgr;
	
	void Initialize();
public:
	const char* GLSL_VERSION = "#version 410";
	const int32_t GL_VERSION_MAJOR = 4;
	const int32_t GL_VERSION_MINOR = 1;

	static Engine& GetInstance();
	WindowManager& GetWindowManager();
	void Start();
};
