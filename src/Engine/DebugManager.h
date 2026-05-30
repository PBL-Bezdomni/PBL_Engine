#pragma once

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <spdlog/spdlog.h>
#include "WindowManager.h"
#include "Game/SceneManager.h"

struct GameObjectData
{
	GameObject* gameObject;
	string Name;
	int ID;
	string ParentName;
	int ParentID;

	string Mesh;
	string DiffuseTex;
	bool HasNormal;
	string NormalTex;
	
	float PosX;
	float PosY;
	float PosZ;

	float RotX;
	float RotY;
	float RotZ;

	float ScaX;
	float ScaY;
	float ScaZ;

	bool HasRigidBody;
	bool HasCollider;
	bool HasTrigger;

	float ColliderSizeX;
	float ColliderSizeY;
	float ColliderSizeZ;

	vector<string> Scripts;

	vector<GameObjectData> Children;
};

class DebugManager
{
public:
	DebugManager() = default;
	~DebugManager();
	void InitializeImGUI(GLFWwindow* window, const char* glslVersion);
	void RefreshGameObjectData();
	
	void RenderImgui(GLFWwindow* window);
private:
	SceneManager*  m_SceneMgr;
	Camera* m_MainCamera;
	
	void ImGUIBegin();
	void ImGUIRender();
	void ImGUIEnd(GLFWwindow* window);

	void LoadCameraData();
	void RenderCameraImgui();
	bool HasCameraUpdated();
	void UpdateCamera();
	void SaveCameraData();
	
	GameObjectData InitializeGameObjectData(GameObject* obj, bool isFirstCall);
	GameObjectData LoadSceneData(GameObject* obj, bool isFirstCall = false);
	void RenderGameObjectsImgui();
	void RenderGameObjectTree(GameObjectData& data);
	bool HasGameObjectsUpdated();
	void UpdateGameObjects();
	void SaveGameObjectsData();

	// Setting fields
	char m_CameraSaveName[255] = "camera";
	float m_CamPositionBorder = 100;
	float m_CamPitchBorder = 90;
	float m_CamYawBorder = 180;
	float m_CamZoomBorderMin = 1;
	float m_CamZoomBorderMax = 80;
	
	float m_CameraPosX = 0;
	float m_PrevCameraPosX = 0;
	float m_CameraPosY = 0;
	float m_PrevCameraPosY = 0;
	float m_CameraPosZ = 0;
	float m_PrevCameraPosZ = 0;
	float m_CameraYaw = 0;
	float m_PrevCameraYaw = 0;
	float m_CameraPitch = 0;
	float m_PrevCameraPitch = 0;
	float m_CameraZoom = 0;
	float m_PrevCameraZoom = 0;

	GameObjectData m_SceneObjectData;
};
