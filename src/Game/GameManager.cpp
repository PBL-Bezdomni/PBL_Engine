#include "GameManager.h"
#include "Engine/WindowManager.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include "Engine/Time.h"
#include "Engine/Engine.h"

void GameManager::Initialize()
{
	m_SceneMgr = std::make_unique<SceneManager>();
}

void GameManager::StartGame()
{
	Engine& engine = Engine::GetInstance();
	Physics = &engine.GetPhysicsEngine();
	WindowMgr = &engine.GetWindowManager();
	
	LoadScene();
	GameLoop();
}

void GameManager::GameLoop()
{
	while (!WindowMgr->ShouldWindowClose())
	{
		// Clear screen
		WindowMgr->ClearFrame();

		// Process I/O operations here
		m_SceneMgr->UpdateScene();
		
		// Update game objects' state here
		Time::Update();

		float frameTime = Time::GetDeltaTime();
		if (frameTime > 0.25f) {
			frameTime = 0.25f;
		}

		physicsAccumulator += frameTime;

		while (physicsAccumulator >= FIXED_TIME_STEP)
		{
			Physics->Update(FIXED_TIME_STEP);
			physicsAccumulator -= FIXED_TIME_STEP;
		}
		
		// OpenGL rendering code here
		m_SceneMgr->RenderScene();


		// End frame and swap buffers (double buffering)
		WindowMgr->EndFrame();
	}
}

void GameManager::LoadScene()
{
	m_SceneMgr->Initialize();
}

