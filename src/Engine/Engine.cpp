#include "Engine.h"

Engine& Engine::GetInstance()
{
	static Engine instance;
	return instance;
}

Engine::Engine()
{
	Initialize();
}

void Engine::Initialize()
{
	// TODO Initialize modules
}

void Engine::Start()
{
	// TODO start engine
}

