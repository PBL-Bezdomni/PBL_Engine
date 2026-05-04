#pragma once
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "FreeType.h"
#include "GameObject.h"
#include "Skybox.h"
#include "SpawnManager.h"
#include "Engine/InputManager.h"

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
	
public:
	int Initialize();
	void UpdateScene();
	void RenderScene();

private:
	//TODO move those functions elsewhere or remove them completely
	void UpdateShaderLight(GameObject* gameObject, Shader& shader);
	void AssignSceneGraph();
	void AssignSceneModelsGraph();
	void LoadSceneModels();
	void LoadModels();
	void init_shader();
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

	Shader m_BasicShader;
	Shader m_SkyboxShader;
	Shader m_UIShader;
	Shader m_TextShader;
	Shader m_SliderShader;
	Shader m_LineShader;
	Shader m_LightSourceShader;

	FreeType m_TextRenderer;

	Texture m_FloorTex;
	Texture m_WallTex;
	Texture m_TableTex;
	Texture m_WaterpoolTex;
	Texture m_TowelsTex;
	Texture m_SliderTex;
	Texture m_UIDuckTex;
	
	GameObject skybox;

	GameObject monkey;
	GameObject duckTransparent;
	GameObject slider;
	GameObject objectsTransform;

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

	InputManager inputManager;
	Player* p1;
	Player* p2;

	float m_Posx = 0;
	float m_Posy = 0;
	float m_Posz = 0;

	// Loader LoadManager = Loader();
	SpawnManager m_SpawnManager;
	Skybox       m_Skybox;

	WindowManager* WindowMgr = nullptr;
	DebugManager*  DebugMgr = nullptr;
	PhysicsEngine* Physics = nullptr;
	AssetManager*  AssetMgr = nullptr;

	const float CAMERA_NEAR_PLANE = .1f;
	const float CAMERA_FAR_PLANE = 200.f;
	const float FLOOR_TEX_SCALE = 8.f;
	const float FLOOR_SCALE = 100.f;
	const int HOUSE_NET_DIM = 200;

	float WALL_X_BORDER = 37.f;
	float WALL_Y_BORDER = 50.f;

	// const float FIXED_TIME_STEP = 1.0f / 60.0f;
	// float physicsAccumulator = 0.0f;

	float m_CurrentRotationDegrees;
	int m_RotationCount;
	float m_SpawnCounter;
	float m_SpawnTime = 2.f;
	bool isDebugDraw;
};
