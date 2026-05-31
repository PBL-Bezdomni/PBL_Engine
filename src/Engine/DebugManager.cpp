#include "DebugManager.h"
#include "Engine/JSONImporter.h"
#include "Engine.h"
#include <memory>

#include "EngineConsts.h"
#include "misc/cpp/imgui_stdlib.h"


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
	m_AssetMgr = &engine->GetAssetManager();
	m_SceneMgr = &engine->GetGameManager().GetSceneManager();
	m_MainCamera = m_SceneMgr->GetMainCamera().get();
	LoadCameraData();
	RefreshGameObjectData();
}

void DebugManager::RefreshGameObjectData()
{
	m_RefreshObjects = false;
	m_SceneObjectData = LoadSceneData(m_SceneMgr->GetLevelParent(), true);
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
	
GameObjectData DebugManager::InitializeGameObjectData(GameObject* obj, bool isFirstCall)
{	
	GameObjectData data;
	data.gameObject = obj;
	data.Name = obj->Name;
	data.ID = obj->ID;
	if (data.ID > m_HighestID)
	{
		m_HighestID = data.ID;
	}
	if (obj->Parent != nullptr && !isFirstCall)
	{
		data.ParentName = obj->Parent->Name;
		data.ParentID = obj->Parent->ID;
	}
	else if (isFirstCall)
	{
		data.ParentName = "";
		data.ParentID = -1;
	}

	Model* model = obj->GetComponent<Model>();
	data.HasNormal = false;
	if (model != nullptr)
	{
		bool hasDiffuse = false;
		bool hasNormal = false;
		data.Mesh = model->GetFileName();
		for (Mesh mesh : model->Meshes)
		{
			for (Texture tex : mesh.Textures)
			{
				if (tex.Type == EngineConsts::DIFFUSE && !hasDiffuse)
				{
					data.DiffuseTex = tex.FileName.c_str();
					hasDiffuse = true;
				}
				if (tex.Type == EngineConsts::NORMAL && !hasNormal)
				{
					data.NormalTex = tex.FileName;
					hasNormal = true;
					data.HasNormal = data.NormalTex != "";
				}
			}
		}
	}

	Transform* transform = obj->transform;
	data.PosX = transform->Position.x;
	data.PosY = transform->Position.y;
	data.PosZ = transform->Position.z;
	data.RotX = transform->EulerAngles.x;
	data.RotY = transform->EulerAngles.y;
	data.RotZ = transform->EulerAngles.z;
	data.ScaX = transform->Scale.x;
	data.ScaY = transform->Scale.y;
	data.ScaZ = transform->Scale.z;

	RigidBody* rb = obj->GetComponent<RigidBody>();
	data.HasRigidBody = false;
	data.IsTrigger = false;
	data.IsStatic = false;
	data.ColliderSizeX = 0;
	data.ColliderSizeY = 0;
	data.ColliderSizeZ = 0;
	if (rb != nullptr)
	{
		data.HasRigidBody = true;
		data.IsTrigger = rb->GetIsTrigger();
		// TODO rework has collider na is static in json
		data.IsStatic = rb->GetIsStatic();
		glm::vec3 hitboxSize = rb->GetHitBoxSize(); 
		data.ColliderSizeX = hitboxSize.x;
		data.ColliderSizeY = hitboxSize.y;
		data.ColliderSizeZ = hitboxSize.z;
	}

	vector<Behaviour*> scripts = obj->GetVectorOfComponents<Behaviour>();
	if (scripts.size() > 0)
	{
		for (int i = 0; i < scripts.size(); i++)
		{
			data.Scripts.emplace_back(scripts[i]->GetScriptName());
		}
	}
	
	return data;
}

GameObjectData DebugManager::LoadSceneData(GameObject* obj, bool isFirstCall)
{
	GameObjectData data = InitializeGameObjectData(obj, isFirstCall);
	for (GameObject* child : obj->Children)
	{
		// Skip animals. TODO may require tweak in the future
		if (child->GetComponent<Animal>() != nullptr) continue;
		// GameObjectData chidlData = InitializeGameObjectData(child);
		GameObjectData childData = LoadSceneData(child);
		data.Children.push_back(childData);
	}
	return data;
}

void DebugManager::RenderGameObjectsImgui()
{
	ImGui::Begin("Scene");
	ImGui::InputText("Scene name", m_SceneSaveName, sizeof(m_SceneSaveName));
	if (ImGui::Button("Save Scene"))
	{
		SaveGameObjectsData();
	}
	RenderGameObjectTree(m_SceneObjectData);
	ImGui::End();

	if (m_RefreshObjects)
	{
		RefreshGameObjectData();
	}
}

bool DebugManager::RenderGameObjectTree(GameObjectData& data)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	bool opened = ImGui::TreeNodeEx((void*)(intptr_t)data.ID, flags, "%s (ID: %d)", data.Name.c_str(), data.ID);
	m_UpdatedGameObject = false;

	if (opened)
	{
		if (ImGui::InputText("Name", &data.Name))
		{
			data.gameObject->Name = data.Name;
		}
		float position[3] = {data.PosX, data.PosY, data.PosZ};
		float rotation[3] = {data.RotX, data.RotY, data.RotZ};
		float scale[3] = {data.ScaX, data.ScaY, data.ScaZ};
		ImGui::Spacing();
		ImGui::Text("Transform");
		if (ImGui::InputFloat3("Position", position))
		{
			data.PosX = position[0];
			data.PosY = position[1];
			data.PosZ = position[2];
			m_UpdatedGameObject = true;
		}
		if (ImGui::InputFloat3("Rotation", rotation))
		{
			data.RotX = rotation[0];
			data.RotY = rotation[1];
			data.RotZ = rotation[2];
			m_UpdatedGameObject = true;
		}
		if (ImGui::InputFloat3("Scale", scale))
		{
			data.ScaX = scale[0];
			data.ScaY = scale[1];
			data.ScaZ = scale[2];
			m_UpdatedGameObject = true;
		}

		if (data.Mesh != "")
		{
			ImGui::Spacing();
			ImGui::Text("Model");
			ImGui::InputText("Model name", &data.Mesh);
			ImGui::InputText("Texture Path", &data.DiffuseTex);
			ImGui::Checkbox("Use Normal", &data.HasNormal);
			if (data.HasNormal)
			{
				ImGui::InputText("Normal Texture Path", &data.NormalTex);
			}
			if (ImGui::Button("Refresh model"))
			{
				if (data.gameObject->GetComponent<Model>()->GetFileName() != data.Mesh)
				{
					data.gameObject->RemoveComponent<Model>();
					Model model = *m_AssetMgr->GetModel(*m_AssetMgr->BasicShader, (EngineConsts::SCENE_MODELS_PATH + data.Mesh).c_str());
					data.gameObject->AddComponent<Model>(model);
				}
				if (data.DiffuseTex != "")
				{
					Texture diff = *m_AssetMgr->GetTexture((EngineConsts::SCENE_TEXTURE_PATH + data.DiffuseTex).c_str());
					data.gameObject->GetComponent<Model>()->AssignTexture(diff);
				}
				if (data.HasNormal && data.NormalTex != "")
				{
					// TODO flag to model to use normals
					Texture norm = *m_AssetMgr->GetTexture((EngineConsts::SCENE_TEXTURE_PATH + data.NormalTex).c_str(), EngineConsts::NORMAL);
					data.gameObject->GetComponent<Model>()->AssignNormal(norm);
				}
				m_RefreshObjects = true;
			}
			if (ImGui::Button("Remove model"))
			{
				data.gameObject->RemoveComponent<Model>();
				Model* model = data.gameObject->GetComponent<Model>();
				if (model == nullptr)
				{
					spdlog::info("Remove Model component successful");
				}
				m_RefreshObjects = true;
			}
		}
		else
		{
			if (ImGui::Button("Add Model"))
			{
				AddModel(data.gameObject);
			}
		}

		if (data.HasRigidBody)
		{
			ImGui::Spacing();
			ImGui::Text("Collider");
			ImGui::Checkbox("Is static", &data.IsStatic);
			ImGui::Checkbox("Is trigger", &data.IsTrigger);
			float size[3] = {data.ColliderSizeX, data.ColliderSizeY, data.ColliderSizeZ};
			if (ImGui::InputFloat3("Collider Size", size))
			{
				data.ColliderSizeX = size[0];
				data.ColliderSizeY = size[1];
				data.ColliderSizeZ = size[2];
				m_UpdatedGameObject = true;
			}
			if (ImGui::Button("Remove Rigid Body"))
			{
				data.gameObject->RemoveComponent<RigidBody>();
				RigidBody* rb = data.gameObject->GetComponent<RigidBody>();
				if (rb == nullptr)
				{
					spdlog::info("Remove component successful");
				}
				m_RefreshObjects = true;
			}
		}
		else
		{
			if (ImGui::Button("Add RigidBody"))
			{
				AddRigidBody(data.gameObject);
			}
		}

		if (HasGameObjectUpdated())
		{
			UpdateGameObjects(data);
		}

		if (ImGui::TreeNode("Scripts"))
		{
			for (size_t i = 0; i < data.Scripts.size(); i++)
			{
				ImGui::PushID(static_cast<int>(i));

				ImGui::Text("Script %zu", i);
				ImGui::SameLine();
				if (ImGui::InputText("##Script", &data.Scripts[i]))
				{
					data.gameObject->AssignScript(data.Scripts[i]);
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove"))
				{
					data.gameObject->RemoveScript(data.Scripts[i]);
					data.Scripts.erase(data.Scripts.begin() + i);
					ImGui::PopID();
					m_RefreshObjects = true;
					break;
				}
				ImGui::PopID();
			}

			if (ImGui::Button("Add Script"))
			{
				data.Scripts.emplace_back("");
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Children"))
		{
			for (size_t i = 0; i < data.Children.size(); i++)
			{
				GameObjectData& childData = data.Children[i];
				bool isOpened = RenderGameObjectTree(childData);
				if (!isOpened)
				{
					ImGui::SameLine();
				}
				ImGui::PushID(static_cast<int>(i));
				if (ImGui::Button("Remove Child"))
				{
					data.gameObject->RemoveChild(childData.gameObject);
					m_RefreshObjects = true;
					ImGui::PopID();
					break;
				}
				ImGui::PopID();
			}

			if (ImGui::Button("Add Child"))
			{
				AddChild(data.gameObject);
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	return opened;
}


bool DebugManager::HasGameObjectUpdated()
{
	return m_UpdatedGameObject;
}

void DebugManager::UpdateGameObjects(GameObjectData& data)
{
	GameObject* gameObject = data.gameObject;
	gameObject->transform->Position = glm::vec3(data.PosX, data.PosY, data.PosZ);
	gameObject->transform->EulerAngles = glm::vec3(data.RotX, data.RotY, data.RotZ);
	gameObject->transform->Scale = glm::vec3(data.ScaX, data.ScaY, data.ScaZ);
	gameObject->UpdateSelfAndChild();
	RigidBody* rb = gameObject->GetComponent<RigidBody>();
	if (rb != nullptr)
	{
		rb->SetHitboxSize(glm::vec3(data.ColliderSizeX, data.ColliderSizeY, data.ColliderSizeZ));
		// rb->SetRotation(gameObject->transform->EulerAngles);
		// rb->Teleport(gameObject->transform->Position);
	}
}

void DebugManager::SaveGameObjectsData()
{
	JSONImporter importer = JSONImporter();
	importer.SaveSceneData(m_SceneSaveName, m_SceneObjectData);
}

void DebugManager::AddChild(GameObject* root)
{
	shared_ptr<GameObject> gameObject = m_SceneMgr->Instantiate(root);
	root->UpdateSelfAndChild();
	gameObject->ID = ++m_HighestID;
	gameObject->Name = "New Object";
	m_RefreshObjects = true;
	// RefreshGameObjectData();
}

void DebugManager::AddRigidBody(GameObject* go)
{
	go->AddComponent<RigidBody>();
	go->GetComponent<RigidBody>()->Init(glm::vec3(1), true);
	m_RefreshObjects = true;
	// RefreshGameObjectData();
}

void DebugManager::AddModel(GameObject* go)
{
	Model model = *m_AssetMgr->GetSphereModel();
	go->AddComponent<Model>(model);
	m_RefreshObjects = true;
	// RefreshGameObjectData();
}
