#pragma once

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include "WindowManager.h"
#include "Events/EventBinder.h"
#include "Game/SceneManager.h"

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
	AssetManager* m_AssetMgr;
	Camera* m_MainCamera;
	int m_HighestID = 0;
	EventBinder m_Binder;
	
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
	bool RenderGameObjectTree(GameObjectData& data);
	bool HasGameObjectUpdated();
	void UpdateGameObjects(GameObjectData& data);
	void SaveGameObjectsData();
	void AddChild(GameObject* root);
	void AddRigidBody(GameObject* go);
	void AddModel(GameObject* go);

	bool m_RefreshObjects = false;
	
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

	bool m_UpdatedGameObject;
	char m_SceneSaveName[255] = "scene";

	GameObjectData m_SceneObjectData;
};
