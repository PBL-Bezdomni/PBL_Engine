#include "DebugManager.h"
#include "Engine/JSONImporter.h"
#include "Engine.h"
#include <memory>


DebugManager::~DebugManager()
{
	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// ImGui::DestroyContext();
}

void DebugManager::InitializeImGUI(GLFWwindow* window, const char* glslVersion)
{
	// Setup Dear ImGui binding
	// glfwMakeContextCurrent(window);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	// Setup style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();
	spdlog::info("Initialized ImGui.");

	Engine* engine = &Engine::GetInstance();
	m_SceneMgr = &engine->GetGameManager().GetSceneManager();
	m_MainCamera = m_SceneMgr->GetMainCamera().get();
	LoadCameraData();
	LoadGameObjectsData();
}

void DebugManager::RefreshGameObjectData()
{
	LoadGameObjectsData();
}

void DebugManager::RenderImgui(GLFWwindow* window)
{
	ImGUIBegin();
	ImGUIRender();
	ImGUIEnd(window);
}

// TODO For some reason, I couldn't pinpoint ImGui window doesn't react to input. Some dependencies are wrong, idk
// I'll leave it for now, but at some point, when we will actually need Debug, it will have to be resolve. One possible solution is to
// move this to WindowManager altogether.
void DebugManager::ImGUIBegin()
{
	// glfwPollEvents();
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DebugManager::ImGUIRender()
{
	RenderCameraImgui();
	RenderGameObjectsImgui();
}

void DebugManager::ImGUIEnd(GLFWwindow* window)
{
	ImGui::Render();
	int display_w, display_h;
	
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &display_w, &display_h);

	glViewport(0, 0, display_w, display_h);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugManager::LoadCameraData()
{
	if (m_MainCamera != nullptr)
	{
		m_CameraPosX = m_MainCamera->GetPosition().x;
		m_PrevCameraPosX = m_CameraPosX;
		m_CameraPosY = m_MainCamera->GetPosition().y;
		m_PrevCameraPosY = m_CameraPosY;
		m_CameraPosZ = m_MainCamera->GetPosition().z;
		m_PrevCameraPosZ = m_CameraPosZ;

		m_CameraPitch = m_MainCamera->GetPitch();
		m_PrevCameraPitch = m_CameraPitch;
		m_CameraYaw = m_MainCamera->GetYaw();
		m_PrevCameraYaw = m_CameraYaw;

		m_CameraZoom = m_MainCamera->GetZoom();
		m_PrevCameraZoom = m_CameraZoom;
	}
}


void DebugManager::RenderCameraImgui()
{
	ImGui::Begin("Camera");
	ImGui::Text("Camera Settings");
	if (ImGui::Button("Save"))
	{
		SaveCameraData();
	}
	if (ImGui::Button("Refresh"))
	{
		LoadCameraData();
	}

	ImGui::InputText("Save Name", m_CameraSaveName, sizeof(m_CameraSaveName));
	ImGui::Spacing();
	ImGui::Text("Camera Position");
	ImGui::DragFloat("Camera Pos.x", &m_CameraPosX, 1, -m_CamPositionBorder, m_CamPositionBorder);
	ImGui::DragFloat("Camera Pos.y", &m_CameraPosY, 1, -m_CamPositionBorder, m_CamPositionBorder);
	ImGui::DragFloat("Camera Pos.z", &m_CameraPosZ, 1, -m_CamPositionBorder, m_CamPositionBorder);
	ImGui::Spacing();
	ImGui::Text("Camera Camera Angles");
	ImGui::DragFloat("Camera Yaw", &m_CameraYaw, 1,-m_CamYawBorder, m_CamYawBorder );
	ImGui::DragFloat("Camera Pitch", &m_CameraPitch, 1, -m_CamPitchBorder, m_CamPitchBorder);
	ImGui::Spacing();
	ImGui::Text("Camera FOV");
	ImGui::DragFloat("Camera Zoom", &m_CameraZoom, 1, m_CamZoomBorderMin, m_CamZoomBorderMax);
	
	ImGui::End();

	UpdateCamera();
}

bool DebugManager::HasCameraUpdated()
{
	glm::vec3 prevCamPosVec = glm::vec3(m_PrevCameraPosX, m_PrevCameraPosY, m_PrevCameraPosZ);
	glm::vec3 currCamPosVec = glm::vec3(m_CameraPosX, m_CameraPosY, m_CameraPosZ);
	return prevCamPosVec != currCamPosVec || m_PrevCameraPitch != m_CameraPitch || m_PrevCameraYaw != m_CameraYaw || m_PrevCameraZoom != m_CameraZoom;
}

void DebugManager::UpdateCamera()
{
	if (m_MainCamera != nullptr && HasCameraUpdated())
	{
		m_MainCamera->SetPosition(glm::vec3(m_CameraPosX, m_CameraPosY, m_CameraPosZ));
		m_MainCamera->SetPitch(m_CameraPitch);
		m_MainCamera->SetYaw(m_CameraYaw);
		m_MainCamera->SetZoom(m_CameraZoom);

		LoadCameraData();
	}
}


void DebugManager::SaveCameraData()
{
	JSONImporter importer = JSONImporter();
	importer.SaveCameraData(m_CameraSaveName, m_MainCamera);
}

void DebugManager::LoadGameObjectsData()
{
	GameObject* level = m_SceneMgr->GetLevelObject();
	LoadObjectChildren(level);	
}
	
void DebugManager::InitializeGameObjectData(GameObject* obj)
{
	GameObjectData data;
	data.gameObject = obj;
	data.Name = obj->Name;
	data.ID = obj->ID;
	data.ParentName = obj->Parent->ID;
	data.ParentID = obj->Parent->ID;

	Model* model = obj->GetComponent<Model>();
	TextureTypeNames texTypeName;
	if (model != nullptr)
	{
		bool hasDiffuse = false;
		bool hasNormal = false;
		data.Mesh = model->GetFileName();
		for (Mesh mesh : model->Meshes)
		{
			for (Texture tex : mesh.Textures)
			{
				if (tex.Type == texTypeName.DIFFUSE && !hasDiffuse)
				{
					data.DiffuseTex = tex.FileName;
					hasDiffuse = true;
				}
				if (tex.Type == texTypeName.NORMAL && !hasNormal)
				{
					data.NormalTex = tex.FileName;
					hasNormal = true;
				}
			}
		}
	}

	Transform* transform = obj->transform;
	
}

void DebugManager::LoadObjectChildren(GameObject* obj)
{
	for (GameObject* child : obj->Children)
	{
		InitializeGameObjectData(child);
		LoadObjectChildren(child);
	}
}

void DebugManager::RenderGameObjectsImgui()
{
}

bool DebugManager::HasGameObjectsUpdated()
{
	return false;
}

void DebugManager::UpdateGameObjects()
{
}

void DebugManager::SaveGameObjectsData()
{
}


