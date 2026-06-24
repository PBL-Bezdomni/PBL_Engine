#pragma once
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "GameObject.h"
#include "Skybox.h"
#include "Game/Scripts/SpawnManager.h"
#include "Engine/InputManager.h"
#include "Engine/JSONImporter.h"
#include "UI/UIManager.h"
#include "Game/Scripts/Animal.h"
#include "Engine/Events/AEvent.h"
#include "UI/LeaderBoard.h"

class ParticleSystem;
class AssetManager;
class PhysicsEngine;
class WindowManager;
class Player;
class CelShading;

class SceneManager
{
private:
	const float TIME_LIMIT = 300.f;
	unique_ptr<JSONImporter> m_JSONImporter;
	GameObject m_WorldParent;
	GameObject m_UIParent;
	UIManager m_UIManager;
	UIPanel m_TimerPanel;
	UIPanel m_MoneyPanel;
	UIPanel m_FpsPanel;
	UIPanel m_ALetterPanel;

	LeaderBoard m_LeaderBoard;
	bool m_ShowLeaderBoard = false;
	std::string m_LeaderInputName;
	bool m_WaitingForLeaderName = false;
	std::string m_LeaderFilePath = "res/leaderboard.txt";

	std::vector<shared_ptr<GameObject>> m_AnimalsList;
	
public:
	void Initialize();
	void LoadScene();
	void UpdateScene();
	void RenderScene();
	void AddAnimal(shared_ptr<GameObject> spawnedEntity);
	GameObject* GetLevelParent();
	ParticleSystem* GetParticleSystem();
	shared_ptr<GameObject> Instantiate(GameObject* parent = nullptr, string path = "", shared_ptr<Shader> shader = nullptr);

	shared_ptr<Camera> GetMainCamera();
	float GetTimeLimit();
	float GetTimeLeft();
	float GetTimeProgressRatio();

	AEvent<> OnSceneLoaded; 
private:
	void UpdateShaderLight(GameObject* gameObject, Shader& shader, Shader& depthShader);
	void AssignSceneGraph();
	void LoadModels();
	void InitializeUI();
	void input(GLFWwindow* window);
	
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseCallbackDispatcher(GLFWwindow* window, double xpos, double ypos);
	void CharCallback(GLFWwindow* window, unsigned int codepoint);
	static void CharCallbackDispatcher(GLFWwindow* window, unsigned int codepoint);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void ScrollCallbackDispatcher(GLFWwindow* window, double xoffset, double yoffset);
	static void JoystickCallback(int jid, int event);

	shared_ptr<Camera> MainCamera;
	glm::mat4 m_TextProjection;

	// Mouse fields
	float m_MouseLastX; // = WINDOW_WIDTH / 2; 
	float m_MouseLastY; // = WINDOW_HEIGHT / 2;
	bool  m_IsFirstMouse = true;
	bool m_RenderHitboxes = false;
	bool m_HasDebugButtonBeenReleased = true;
	
	FreeType m_TextRenderer;
	
	Texture m_UIPanelTex;
	Texture m_UICoinTex;
	Texture m_UILetterTex;

	Texture m_DimTex;

	vector<shared_ptr<GameObject>> m_GameObjects;

	unsigned int m_DynamicDepthMap;
	unsigned int m_StaticDepthMap;
	bool m_HasStaticMapLoaded = false;
	bool m_GameOverTriggered = false;
	
	GameObject skybox;

	GameObject m_ParticleSystem;
	GameObject m_CameraManager;

	GameObject m_LightSource;
	GameObject m_LightSourceObject;
	
	GameObject m_Player1;
	GameObject m_Player2;

	CelShading* m_cl;
	
	Skybox m_Skybox;

	WindowManager* WindowMgr = nullptr;
	PhysicsEngine* Physics = nullptr;
	AssetManager*  AssetMgr = nullptr;
	
	float m_TimeLeft = TIME_LIMIT;

	GameObject bath;
};
