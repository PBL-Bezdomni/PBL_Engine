#pragma once
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "GameObject.h"
#include "Skybox.h"
#include "Game/Scripts/SpawnManager.h"
#include "Engine/InputManager.h"
#include "UI/UIManager.h"

class AssetManager;
class PhysicsEngine;
class DebugManager;
class WindowManager;
class Player;

class SceneManager
{
private:
	GameObject m_WorldParent;
	GameObject m_UIParent;
	UIManager m_UIManager;
	UIPanel m_TimerPanel;
	UIPanel m_MoneyPanel;
	UIPanel m_FpsPanel;
	
public:
	int Initialize();
	void UpdateScene();
	void RenderScene();
	void AddAnimal(shared_ptr<GameObject> spawnedEntity);
	shared_ptr<GameObject> Instantiate(string path, shared_ptr<Shader> shader = nullptr);

	shared_ptr<Camera> GetMainCamera();

private:
	//TODO move those functions elsewhere or remove them completely
	void UpdateShaderLight(GameObject* gameObject, Shader& shader);
	void AssignSceneGraph();
	void AssignSceneModelsGraph();
	void LoadSceneModels();
	void LoadModels();
	void InitializeUI();
	void input(GLFWwindow* window);
	
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseCallbackDispatcher(GLFWwindow* window, double xpos, double ypos);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void ScrollCallbackDispatcher(GLFWwindow* window, double xoffset, double yoffset);
	static void JoystickCallback(int jid, int event);
	// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	// static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	shared_ptr<Camera> MainCamera;

	// Mouse fields
	float m_MouseLastX; // = WINDOW_WIDTH / 2; 
	float m_MouseLastY; // = WINDOW_HEIGHT / 2;
	bool  m_IsFirstMouse = true;


	FreeType m_TextRenderer;

	Texture m_FloorTex;
	Texture m_FloorNorm;
	Texture m_WallTex;
	Texture m_TableTex;
	Texture m_WaterpoolTex;
	Texture m_TowelsTex;
	Texture m_UISliderTex;
	Texture m_UIDuckTex;
	Texture m_UIPanelTex;
	Texture m_UICoinTex;

	vector<shared_ptr<GameObject>> m_GameObjects;
	
	GameObject skybox;

	GameObject monkey;
	GameObject objectsTransform;
	GameObject spawnManagerObject;

	GameObject m_Scene;
	GameObject m_Floor;
	GameObject m_WallDir;
	GameObject m_WallLeft;
	GameObject m_WallRight;
	GameObject m_WallBack;
	GameObject m_WallFrontRight;
	GameObject m_WallFrontLeft;
	GameObject m_OnsenObjects;
	GameObject m_TowelsBed;
	GameObject m_Waterpool;
	GameObject m_TablesDir;
	GameObject m_Table1;
	GameObject m_Table2;
	GameObject m_Table3;
	GameObject m_Table4;
	GameObject m_Table5;

	GameObject m_Ball1;

	GameObject m_LightSource;
	GameObject m_LightSourceObject;

	InputManager* inputManager;
	Player* p1;
	Player* p2;

	float m_Posx = 0;
	float m_Posy = 0;
	float m_Posz = 0;

	// Loader LoadManager = Loader();
	SpawnManager* m_SpawnManager;
	Skybox       m_Skybox;

	WindowManager* WindowMgr = nullptr;
	DebugManager*  DebugMgr = nullptr;
	PhysicsEngine* Physics = nullptr;
	AssetManager*  AssetMgr = nullptr;
	
	const float FLOOR_TEX_SCALE = 8.f;
	const float FLOOR_SCALE = 100.f;
	const int HOUSE_NET_DIM = 200;

	float WALL_X_BORDER = 37.f;
	float WALL_Y_BORDER = 50.f;

	float m_CurrentRotationDegrees;
	int m_RotationCount;
	float m_SpawnCounter;
	float m_SpawnTime = 2.f;
	bool isDebugDraw;
	float m_TimeLeft = 300.0f;
};
