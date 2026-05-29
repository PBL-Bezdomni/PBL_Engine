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

class AssetManager;
class PhysicsEngine;
class DebugManager;
class WindowManager;
class Player;

class SceneManager
{
private:
	const float TIME_LIMIT = 60.f;
	unique_ptr<JSONImporter> m_JSONImporter;
	GameObject m_WorldParent;
	GameObject m_UIParent;
	UIManager m_UIManager;
	UIPanel m_TimerPanel;
	UIPanel m_MoneyPanel;
	UIPanel m_FpsPanel;

	std::vector<shared_ptr<GameObject>> m_AnimalsList;
	
public:
	void Initialize();
	void LoadScene();
	void UpdateScene();
	void RenderScene();
	void AddAnimal(shared_ptr<GameObject> spawnedEntity);
	GameObject* GetLevelParent();
	shared_ptr<GameObject> Instantiate(GameObject* parent = nullptr, string path = "", shared_ptr<Shader> shader = nullptr);
	GameObject* GetLevelObject();

	shared_ptr<Camera> GetMainCamera();
	float GetTimeLimit();
	float GetTimeLeft();

private:
	//TODO move those functions elsewhere or remove them completely
	void UpdateShaderLight(GameObject* gameObject, Shader& shader, Shader& depthShader);
	void AssignSceneGraph();
	void LoadModels();
	void InitializeUI();
	void input(GLFWwindow* window);
	
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseCallbackDispatcher(GLFWwindow* window, double xpos, double ypos);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void ScrollCallbackDispatcher(GLFWwindow* window, double xoffset, double yoffset);
	static void JoystickCallback(int jid, int event);

	shared_ptr<Camera> MainCamera;

	// Mouse fields
	float m_MouseLastX; // = WINDOW_WIDTH / 2; 
	float m_MouseLastY; // = WINDOW_HEIGHT / 2;
	bool  m_IsFirstMouse = true;
	
	FreeType m_TextRenderer;
	
	Texture m_UIPanelTex;
	Texture m_UICoinTex;

	vector<shared_ptr<GameObject>> m_GameObjects;
	
	GameObject skybox;

	GameObject m_LightSource;
	GameObject m_LightSourceObject;
	
	GameObject m_Player1;
	GameObject m_Player2;
	
	Skybox m_Skybox;

	WindowManager* WindowMgr = nullptr;
	PhysicsEngine* Physics = nullptr;
	AssetManager*  AssetMgr = nullptr;
	
	float m_TimeLeft = TIME_LIMIT;

	GameObject bath;
};
