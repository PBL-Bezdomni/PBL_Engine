#include "SceneManager.h"
#include "Quaternion.h"
#include "Random.h"
#include "Engine/Engine.h"
#include "Engine/Components/RigidBody.h"
#include "Game/Scripts/SpawnManager.h"
#include "Engine/AssetManager.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include "Engine/Time.h"
#include "Engine/WindowManager.h"
#include "Game/Scripts/Player.h"
#include "Engine/InputManager.h"
#include "Engine/Light/DirectionalLight.h"
#include "Engine/Light/LightSource.h"
#include "Engine/Light/PointLight.h"
#include "Engine/Light/SpotLight.h"
#include "Engine/Loader.h"

#include "Engine/JSONImporter.h"
#include "Engine/Components/ParticleSystem.h"
#include "Engine/Components/TargetingZone.h"

#include "Game/Scripts/MassageTable.h"
#include "Game/Scripts/Bath.h"
#include "Game/Scripts/Animal.h"
#include "Scripts/CameraManager.h"

#include "CelShading.h"


#define _USE_MATH_DEFINES

void SceneManager::Initialize()
{
	Engine& engine = Engine::GetInstance();

	Physics = &engine.GetPhysicsEngine();
	WindowMgr = &engine.GetWindowManager();
	AssetMgr = &engine.GetAssetManager();

	if (!engine.GetWindowManager().GetIsInitialized())
	{
		std::cout << "ERROR: Failed to initialize project!\n";
		return;
	}
	
	m_JSONImporter = make_unique<JSONImporter>();
	m_JSONImporter->Initialize();

	MainCamera = make_shared<Camera>(glm::vec3(0.f, 25.f, 47.f), glm::vec3(0.0, 1.0, 0.0), -90.f, -25.f);
	MainCamera->SetAspect(WindowMgr->GetWindowAspectRatio());
	m_JSONImporter->LoadCameraData("camera", MainCamera.get());

	m_ParticleSystem = GameObject();
	m_ParticleSystem.AddComponent<ParticleSystem>();
	
	// glfwSetInputMode(WindowMgr->GetWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(WindowMgr->GetWindowPointer(), this);
	glfwSetCursorPosCallback(WindowMgr->GetWindowPointer(), SceneManager::MouseCallbackDispatcher);
	glfwSetScrollCallback(WindowMgr->GetWindowPointer(), ScrollCallbackDispatcher);
	glfwSetJoystickCallback(SceneManager::JoystickCallback);

	m_cl = new CelShading(WindowMgr->GetWindowPointer());
    
	std::cout << "Initialized project.\n";

	std::string fontPath = Loader::RelativePath() + "res/fonts/Choko Regular_8337.otf";
	
	m_UIManager.Init(AssetMgr, WindowMgr, fontPath.c_str());
}

void SceneManager::LoadScene()
{
	m_MouseLastX = WindowMgr->GetWindowWidth() / 2; 
	m_MouseLastY = WindowMgr->GetWindowHeight() / 2;
	
	LoadModels();
	AssignSceneGraph();
	InitializeUI();

	// UI Init
	//m_UIManager.Init(AssetMgr, WindowMgr);

	glEnable(GL_DEPTH_TEST);

	m_Skybox = Skybox(*AssetMgr->SkyboxShader);

	m_WorldParent.UpdateSelfAndChild();
	m_WorldParent.StartSelfAndChild();
	
	m_Player1 = GameObject();
	m_Player1.AddComponent<Player>(0);
	m_Player2 = GameObject();
	m_Player2.AddComponent<Player>(1);

	m_WorldParent.AddChild(&m_Player1);
	m_WorldParent.AddChild(&m_Player2);
	m_Player1.transform->Position = glm::vec3(10.0f, 0.0f, -20.0f);
	m_Player2.transform->Position = glm::vec3(10.0f, 0.0f, -25.0f);

	m_WorldParent.UpdateSelfAndChild();

	m_Player1.AddComponent<RigidBody>();
	m_Player1.GetComponent<RigidBody>()->Init(glm::vec3(1.0f, 1.0f, 1.0f), false);

	m_Player2.AddComponent<RigidBody>();
	m_Player2.GetComponent<RigidBody>()->Init(glm::vec3(1.0f, 1.0f, 1.0f), false);

	GameObject* p1_Zone1 = new GameObject();
	p1_Zone1->Name = "TargetZone_Large_Air";
	p1_Zone1->transform->Position = m_Player1.transform->Position;
	m_Player1.AddChild(p1_Zone1);
	TargetingZone* p1_tz1 = p1_Zone1->AddComponent<TargetingZone>();
	p1_tz1->BaseWeight = 100.0f;
	RigidBody* rb1 = p1_Zone1->AddComponent<RigidBody>();
	rb1->Init(glm::vec3(7.0f, 20.0f, 7.0f), true, true);

	GameObject* p1_Zone2 = new GameObject();
	p1_Zone2->Name = "TargetZone_Small_Proximity";
	p1_Zone2->transform->Position = m_Player1.transform->Position;
	m_Player1.AddChild(p1_Zone2);
	TargetingZone* p1_tz2 = p1_Zone2->AddComponent<TargetingZone>();
	p1_tz2->BaseWeight = 300.0f;
	RigidBody* rb2 = p1_Zone2->AddComponent<RigidBody>();
	rb2->Init(glm::vec3(3.0f, 3.0f, 3.0f), true, true);

	GameObject* p1_Zone3 = new GameObject();
	p1_Zone3->Name = "TargetZone_Medium_Front";
	p1_Zone3->transform->Position = glm::vec3(0.0f, 0.0f, 2.0f);
	m_Player1.AddChild(p1_Zone3);
	TargetingZone* p1_tz3 = p1_Zone3->AddComponent<TargetingZone>();
	p1_tz3->BaseWeight = 500.0f;
	RigidBody* rb3 = p1_Zone3->AddComponent<RigidBody>();
	rb3->Init(glm::vec3(3.0f, 2.0f, 3.0f), true, true);

	GameObject* p1_Zone4 = new GameObject();
	p1_Zone4->Name = "TargetZone_Precision_Front";
	p1_Zone4->transform->Position = glm::vec3(0.0f, 0.0f, 3.0f);
	m_Player1.AddChild(p1_Zone4);
	TargetingZone* p1_tz4 = p1_Zone4->AddComponent<TargetingZone>();
	p1_tz4->BaseWeight = 800.0f;
	RigidBody* rb4 = p1_Zone4->AddComponent<RigidBody>();
	rb4->Init(glm::vec3(0.5f, 2.0f, 4.0f), true, true);



	GameObject* p2_Zone1 = new GameObject();
	p2_Zone1->Name = "TargetZone_Large_Air";
	p2_Zone1->transform->Position = m_Player2.transform->Position;
	m_Player2.AddChild(p2_Zone1);
	TargetingZone* p2_tz1 = p2_Zone1->AddComponent<TargetingZone>();
	p2_tz1->BaseWeight = 100.0f;
	RigidBody* rb5 = p2_Zone1->AddComponent<RigidBody>();
	rb5->Init(glm::vec3(7.0f, 20.0f, 7.0f), true, true);

	GameObject* p2_Zone2 = new GameObject();
	p2_Zone2->Name = "TargetZone_Small_Proximity";
	p2_Zone2->transform->Position = m_Player2.transform->Position;
	m_Player2.AddChild(p2_Zone2);
	TargetingZone* p2_tz2 = p2_Zone2->AddComponent<TargetingZone>();
	p2_tz2->BaseWeight = 300.0f;
	RigidBody* rb6 = p2_Zone2->AddComponent<RigidBody>();
	rb6->Init(glm::vec3(3.0f, 3.0f, 3.0f), true, true);

	GameObject* p2_Zone3 = new GameObject();
	p2_Zone3->Name = "TargetZone_Medium_Front";
	p2_Zone3->transform->Position = glm::vec3(0.0f, 0.0f, 2.0f);
	m_Player2.AddChild(p2_Zone3);
	TargetingZone* p2_tz3 = p2_Zone3->AddComponent<TargetingZone>();
	p2_tz3->BaseWeight = 500.0f;
	RigidBody* rb7 = p2_Zone3->AddComponent<RigidBody>();
	rb7->Init(glm::vec3(3.0f, 2.0f, 3.0f), true, true);

	GameObject* p2_Zone4 = new GameObject();
	p2_Zone4->Name = "TargetZone_Precision_Front";
	p2_Zone4->transform->Position = glm::vec3(0.0f, 0.0f, 3.0f);
	m_Player2.AddChild(p2_Zone4);
	TargetingZone* p2_tz4 = p2_Zone4->AddComponent<TargetingZone>();
	p2_tz4->BaseWeight = 800.0f;
	RigidBody* rb8 = p2_Zone4->AddComponent<RigidBody>();
	rb8->Init(glm::vec3(0.5f, 2.0f, 4.0f), true, true);
	
	// SHADOW
	AssetMgr->BasicShader->Use();
	AssetMgr->BasicShader->SetInt("shadowMap", 20);
	AssetMgr->BasicShader->SetInt("staticShadowMap", 21);
	AssetMgr->AnimatedShader->Use();
	AssetMgr->AnimatedShader->SetInt("shadowMap", 20);
	AssetMgr->AnimatedShader->SetInt("staticShadowMap", 21);
	
	OnSceneLoaded.Invoke();

	m_CameraManager.GetComponent<CameraManager>()->AssignCameraAndPlayer(MainCamera.get(), &m_Player1, &m_Player2);
}

void SceneManager::UpdateScene()
{
	input(WindowMgr->GetWindowPointer());
}


void SceneManager::RenderScene()
{

	AssetMgr->BasicShader->Use();
	AssetMgr->BasicShader->SetBool("useDirLight", false);
	AssetMgr->BasicShader->SetBool("usePointLight", false);
	AssetMgr->BasicShader->SetBool("useSpotLight1", false);

    UpdateShaderLight(&m_WorldParent, *AssetMgr->BasicShader, *AssetMgr->SimpleDepthShader);
	AssetMgr->AnimatedShader->Use();
	AssetMgr->AnimatedShader->SetBool("useDirLight", false);
	AssetMgr->AnimatedShader->SetBool("usePointLight", false);
	AssetMgr->AnimatedShader->SetBool("useSpotLight1", false);
	UpdateShaderLight(&m_WorldParent, *AssetMgr->AnimatedShader, *AssetMgr->SimpleDepthShader);

	m_cl->CheckScreenSize(WindowMgr->GetWindowPointer());
	m_cl->FBOInit();

	glEnable(GL_DEPTH_TEST);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(MainCamera->GetViewMatrix()));
    glm::mat4 view = MainCamera->GetViewMatrix();
    glm::mat4 projection = MainCamera->GetProjectionMatrix();

	AssetMgr->SetShadersViewProjection(view, projection);

    m_WorldParent.UpdateSelfAndChild();

	int windowH, windowW;
	glfwGetWindowSize(WindowMgr->GetWindowPointer(), &windowW, &windowH);
	glViewport(0, 0, windowW, windowH);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_WorldParent.DrawSelfAndChild();

	m_Skybox.DrawSkybox(skyboxView, projection);

    glDisable(GL_DEPTH_TEST);

    if (m_RenderHitboxes && Physics != nullptr)
    {
        Physics->DrawHitboxes(*AssetMgr->LineShader, view, projection);
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_cl->RenderQuad(*AssetMgr->CelShadingShader);

	// IMPORTANT: Do not write things below Freetype/UI, if you do not know what you are doing, thanks :)
	// Draw UI

	// Reapply text projection
	AssetMgr->TextShader->Use();
	AssetMgr->TextShader->SetMat4("projection", m_TextProjection);
	
	// TIMER move it somewhere else later, if you know where then please move it.
	if (m_TimeLeft > 0.0f)
	{
		m_TimeLeft -= Time::GetDeltaTime();
		int minutes = static_cast<int>(m_TimeLeft) / 60;
		int seconds = static_cast<int>(m_TimeLeft) % 60;
		m_TimerPanel.Text = std::to_wstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + std::to_wstring(seconds);
	}
	else
	{
		m_TimerPanel.Text = L"00:00";
	}
	if (SpawnManager::Instance != nullptr)
	{
		m_MoneyPanel.Text = std::to_wstring(SpawnManager::Instance->GetMoneyAnimated());
	}
	else
	{
		m_MoneyPanel.Text = L"ERROR (nullptr)";
		std::cout << "ERROR: SpawnManager::Instance is Null. Logic does not work.\n";
	}
	m_FpsPanel.Text = L"FPS: " + std::to_wstring(Time::GetFPS());

	m_UIManager.DrawPanelWithText(*AssetMgr->UIShader, *AssetMgr->TextShader, m_MoneyPanel);
	m_UIManager.DrawPanelWithText(*AssetMgr->UIShader, *AssetMgr->TextShader, m_TimerPanel);
	m_UIManager.DrawPanelWithText(*AssetMgr->UIShader, *AssetMgr->TextShader, m_FpsPanel);

	GameObject* targetAnimal = nullptr;

	Player* player1Script = m_Player1.GetDerivedComponent<Player>();
	Player* player2Script = m_Player2.GetDerivedComponent<Player>();

	if (player1Script != nullptr && player1Script->GetBestAnimalTarget() != nullptr)
	{
		targetAnimal = player1Script->GetBestAnimalTarget();
	}
	else if (player2Script != nullptr && player2Script->GetBestAnimalTarget() != nullptr)
	{
		targetAnimal = player2Script->GetBestAnimalTarget();
	}

	if (targetAnimal != nullptr)
	{
		glm::vec3 animalWorldPos = targetAnimal->GetWorldPosition() + glm::vec3(0.0f, 3.0f, 0.0f);

		
		int windowW, windowH;
		glfwGetWindowSize(WindowMgr->GetWindowPointer(), &windowW, &windowH);
		glm::vec4 viewport = glm::vec4(0.0f, 0.0f, static_cast<float>(windowW), static_cast<float>(windowH));

		glm::vec3 screenPos = glm::project(animalWorldPos, view, projection, viewport);
		screenPos.y = static_cast<float>(windowH) - screenPos.y;
		m_ALetterPanel.Position = glm::vec2(screenPos.x, screenPos.y);

		m_ALetterPanel.Position.y += sin(glfwGetTime() * 5.0f) * 5.0f;

		m_UIManager.DrawPanelWithText(*AssetMgr->UIShader, *AssetMgr->TextShader, m_ALetterPanel);
	}

}

void SceneManager::AddAnimal(shared_ptr<GameObject> spawnedEntity)
{
	if (spawnedEntity != nullptr)
	{
		m_AnimalsList.push_back(spawnedEntity);
	}
}

GameObject* SceneManager::GetLevelParent()
{
	if (m_WorldParent.Children.size() > 0)
	{
		for (GameObject* child: m_WorldParent.Children)
		{
			if (child != nullptr && child->Name == "Level")
			{
				return child;
			}
		}
	}
	return &m_WorldParent;
}

ParticleSystem* SceneManager::GetParticleSystem()
{
	return m_ParticleSystem.GetComponent<ParticleSystem>();
}

shared_ptr<GameObject> SceneManager::Instantiate(GameObject* parent, string path, shared_ptr<Shader> shader)
{
	// TODO make load components data from prefab path instead of passing model path
	shared_ptr<GameObject> go = make_shared<GameObject>();
	if (path != "")
	{
		if (shader == nullptr)
		{
			shader = AssetMgr->BasicShader;
		}
		Model model = *AssetMgr->GetModel(*shader, path.c_str());
		Model* modelComp = go->AddComponent<Model>(model);
		// Force that shader
		if (modelComp != nullptr)
		{
			modelComp->ReassignShader(*shader);
		}
	}
	if (parent == nullptr)
	{
		parent = &m_WorldParent;
	}
	parent->AddChild(go.get());
	m_GameObjects.push_back(go);
	return m_GameObjects.back();
}

shared_ptr<Camera> SceneManager::GetMainCamera()
{
	return MainCamera;
}

float SceneManager::GetTimeLimit()
{
	return TIME_LIMIT;
}

float SceneManager::GetTimeLeft()
{
	return m_TimeLeft;
}

float SceneManager::GetTimeProgressRatio()
{
	return m_TimeLeft / TIME_LIMIT;
}

void SceneManager::UpdateShaderLight(GameObject* gameObject, Shader& shader, Shader& depthShader)
{
	if (gameObject == nullptr)
	{
		return;
	}
	
	LightSource* dLight = gameObject->GetComponent<DirectionalLight>();
	if (dLight != nullptr)
	{
		dLight->SetLightValues(shader);
		glm::mat4 lightSpaceMatrix(0.0f);
		m_DynamicDepthMap = dLight->GetDynamicShadowMap(m_WorldParent, depthShader);
		if (!m_HasStaticMapLoaded)
		{
			m_StaticDepthMap = dLight->GetStaticShadowMap(m_WorldParent, depthShader);
			m_HasStaticMapLoaded = true;
		}
		lightSpaceMatrix = dLight->getLightViewMatrix();
		AssetMgr->BasicShader->Use();

		AssetMgr->BasicShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_2D, m_DynamicDepthMap);
		glActiveTexture(GL_TEXTURE21);
		glBindTexture(GL_TEXTURE_2D, m_StaticDepthMap);
	}
	LightSource* pLight = gameObject->GetComponent<PointLight>();
	if (pLight != nullptr)
	{
		pLight->SetLightValues(shader);
	}
	LightSource* sLight = gameObject->GetComponent<SpotLight>();
	if (sLight != nullptr)
	{
		sLight->SetLightValues(shader);
	}
	for (int i = 0; i < gameObject->Children.size(); i++)
	{
		UpdateShaderLight(gameObject->Children[i], shader, depthShader);
	}
}

void SceneManager::AssignSceneGraph()
{
	m_WorldParent.AddChild(&m_LightSource);
	m_WorldParent.AddChild(&m_ParticleSystem);
	m_WorldParent.AddChild(&m_CameraManager);

	// m_WorldParent.transform->Position = glm::vec3(0.f, 0.f, -30.f);
	// m_WorldParent.transform->Scale = glm::vec3(0.7f);

	m_WorldParent.UpdateSelfAndChild();
}

void SceneManager::LoadModels()
{
	m_WorldParent = GameObject();
	vector<shared_ptr<GameObject>> objs = m_JSONImporter->ImportScene("scene2", &m_WorldParent);
	m_GameObjects.insert(m_GameObjects.end(), objs.begin(), objs.end());

	m_LightSourceObject = GameObject();
	Model lightModel = *AssetMgr->GetModel(*AssetMgr->LightSourceShader, "res/models/sphere/ball.obj");
	m_LightSourceObject.AddComponent<Model>(lightModel);
	
	m_LightSource = GameObject();
	m_LightSource.AddComponent<DirectionalLight>(MainCamera, m_LightSource.transform, glm::vec3(0.0f, -1.0f, -1.0f));

	m_LightSource.transform->Position = glm::vec3(0.f, 15.0f, 0.0f);
	//m_LightSource.transform->Position = glm::vec3(0.f, 15.0f, -30.0f);

	m_UIPanelTex = *AssetMgr->GetTexture("res/textures/UI/UI_panel.png");
	m_UICoinTex = *AssetMgr->GetTexture("res/textures/UI/coin.png");
	m_UILetterTex = *AssetMgr->GetTexture("res/textures/UI/Xbox_button_A.png");

	m_CameraManager = GameObject();
	m_CameraManager.Name = "CameraManager";
	m_CameraManager.AddComponent<CameraManager>();
}

void SceneManager::InitializeUI()
{
	m_TextProjection = glm::ortho(0.0f, WindowMgr->GetWindowWidth(), WindowMgr->GetWindowHeight(), 0.0f);
	AssetMgr->TextShader->Use();
	AssetMgr->TextShader->SetMat4("projection", m_TextProjection);

	m_MoneyPanel.TextureID = m_UIPanelTex.ID;
	m_MoneyPanel.Size = glm::vec2(300.0f, 100.0f);
	m_MoneyPanel.Position = glm::vec2(10.0f, 10.0f);
	m_MoneyPanel.TextColor = glm::vec3(0.333f, 0.227f, 0.196f);
	m_MoneyPanel.HasIcon = true;
	m_MoneyPanel.InconTextureID = m_UICoinTex.ID;
	m_MoneyPanel.IconSize = glm::vec2(40.0f, 40.0f);

	m_ALetterPanel.TextureID = m_UILetterTex.ID;
	m_ALetterPanel.HasTexture = true;
	m_ALetterPanel.Size = glm::vec2(50.0f, 50.0f);
	m_ALetterPanel.Position = glm::vec2((WindowMgr->GetWindowWidth() / 2.0f) - 50.0f, (WindowMgr->GetWindowHeight() / 2.0f) - 50.0f);
	m_ALetterPanel.HasIcon = true;
	m_ALetterPanel.InconTextureID = m_UILetterTex.ID;
	m_ALetterPanel.TextScale = 1.0f;

	m_TimerPanel.TextureID = m_UIPanelTex.ID;
	m_TimerPanel.Size = glm::vec2(300.0f, 100.0f);
	m_TimerPanel.Position = glm::vec2((WindowMgr->GetWindowWidth() / 2.0f) - 150.0f, 10.0f);
	m_TimerPanel.TextColor = glm::vec3(0.333f, 0.227f, 0.196f);

	m_FpsPanel.HasTexture = false;
	m_FpsPanel.Size = glm::vec2(300.0f, 100.0f);
	m_FpsPanel.Position = glm::vec2(WindowMgr->GetWindowWidth() - 300.0f, 10.0f);
	m_FpsPanel.TextColor = glm::vec3(0.333f, 0.227f, 0.196f);

}

void SceneManager::input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		MainCamera->ProcessKeyboard(FORWARD, Time::GetDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		MainCamera->ProcessKeyboard(BACKWARD, Time::GetDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		MainCamera->ProcessKeyboard(LEFT, Time::GetDeltaTime());
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		MainCamera->ProcessKeyboard(RIGHT, Time::GetDeltaTime());
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Simulate mouse movement with arrows.
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		MainCamera->ProcessMouseMovement(0, 1.f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		MainCamera->ProcessMouseMovement(0, -1.f);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		MainCamera->ProcessMouseMovement(1.f, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		MainCamera->ProcessMouseMovement(-1.f, 0);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			if (m_HasDebugButtonBeenReleased)
			{
				// Shift + E, toggle Editor
				Engine::GetInstance().ToggleDebugDraw();
				m_HasDebugButtonBeenReleased = false;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		{
			if (m_HasDebugButtonBeenReleased)
			{
				// Shift + H, toggle hitboxes
				m_RenderHitboxes = !m_RenderHitboxes;
				m_HasDebugButtonBeenReleased = false;
			}
		}
		else
		{
			m_HasDebugButtonBeenReleased = true;
		}
	}
}

void SceneManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (m_IsFirstMouse)
		{
			m_MouseLastX = xpos;
			m_MouseLastY = ypos;
			m_IsFirstMouse = false;
		}

		float xoffset = xpos - m_MouseLastX;
		float yoffset = m_MouseLastY - ypos; // reversed since y-coordinates range from bottom to top
		m_MouseLastX = xpos;
		m_MouseLastY = ypos;

		MainCamera->ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		m_IsFirstMouse = true;
	}
}

void SceneManager::MouseCallbackDispatcher(GLFWwindow* window, double xpos, double ypos)
{
	SceneManager* self = static_cast<SceneManager*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->MouseCallback(window, xpos, ypos);
	}
}

void SceneManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		MainCamera->ProcessMouseScroll(yoffset);
	}
}

void SceneManager::ScrollCallbackDispatcher(GLFWwindow* window, double xoffset, double yoffset)
{
	SceneManager* self = static_cast<SceneManager*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->ScrollCallback(window, xoffset, yoffset);
	}
}

void SceneManager::JoystickCallback(int jid, int event) {
	if (event == GLFW_CONNECTED) {
		std::cout << "Joystick connected: " << glfwGetJoystickName(jid) << std::endl;
	}
	else if (event == GLFW_DISCONNECTED) {
		std::cout << "WARNING: Joystick disconnected: " << std::to_string(jid) << std::endl;
	}
}
