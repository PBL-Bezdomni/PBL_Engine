#pragma once
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "FreeType.h"
#include "GameObject.h"
#include "Skybox.h"
#include "SpawnManager.h"

class AssetManager;
class PhysicsEngine;
class DebugManager;
class WindowManager;

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
	void SetupShaderLight(Shader& shader);
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
	// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	// static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	Camera MainCamera;

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

	// Values for light
	bool m_UseDirLight = false;
	bool m_UsePointLight = true;
	bool m_UseSpotLight1 = false;
	bool m_UseSpotLight2 = false;

	glm::vec3 m_PointLightPos = glm::vec3(0.f, 15.0f, -30.0f);
	glm::vec3 m_PointLightColor = glm::vec3(1.f, 1.f, 1.f);
	float     m_PointLightRadius = 100.f;
	float     m_PointLightHeight = 5.f;
	float     m_PointLightLinear = 0.0014f;
	float     m_PointLightQuadratic = 0.000007f;

	glm::vec3 m_DirLightDirection = glm::vec3(0.f, -1.0f, 0.f);
	glm::vec3 m_DirLightVisualPos = glm::vec3(0.f, 10.f, 0.f);
	glm::vec3 m_DirLightColor = glm::vec3(1.f, 1.f, 1.f);
	float     m_DirLightAngleX = -45.f;
	float     m_DirLightAngleZ = 30.f;

	glm::vec3 m_SpotLight1Color = glm::vec3(1.f, 1.f, 1.f);
	glm::vec3 m_SpotLight1Position = glm::vec3(0.0f, 0.0f, 4.0f);
	glm::vec3 m_SpotLight1Direction = glm::vec3(0.0f, 0.0f, 1.0f);
	float     m_SpotLight1CutOff = 24.5f;
	float     m_SpotLight1OuterCutOff = 30.5f;
	float     m_SpotLight1Angle = 0.f;
	float     m_SpotLight1Linear = 0.09f;
	float     m_SpotLight1Quadratic = 0.032f;

	glm::vec3 m_SpotLight2Color = glm::vec3(1.f, 1.f, 1.f);
	glm::vec3 m_SpotLight2Position = glm::vec3(8.f, 0.0f, -6.0f);
	glm::vec3 m_SpotLight2Direction = glm::vec3(0.0f, 0.0f, -1.0f);
	float     m_SpotLight2CutOff = 8.5f;
	float     m_SpotLight2OuterCutOff = 12.5f;
	float     m_SpotLight2Angle = 135.f;
	float     m_SpotLight2Linear = 0.09f;
	float     m_SpotLight2Quadratic = 0.032f;

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
