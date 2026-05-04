#include "SceneManager.h"
#include "Quaternion.h"
#include "Random.h"
#include "Engine/Engine.h"
#include "Engine/Components/RigidBody.h"

#include "Engine/AssetManager.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include "Engine/DebugManager.h"
#include "Engine/Time.h"
#include "Engine/WindowManager.h"
#include "Player.h"
#include "Engine/InputManager.h"
#include "Engine/Light/DirectionalLight.h"
#include "Engine/Light/LightSource.h"
#include "Engine/Light/PointLight.h"
#include "Engine/Light/SpotLight.h"

#define _USE_MATH_DEFINES

int SceneManager::Initialize()
{
	Engine& engine = Engine::GetInstance();
	isDebugDraw = engine.GetIsDebugDrawn();

	WindowMgr = &engine.GetWindowManager();
	AssetMgr = &engine.GetAssetManager();

	m_SpawnManager.Initialize();
	
	// engine.Start();
	if (!engine.GetWindowManager().GetIsInitialized())
	{
		spdlog::error("Failed to initialize project!");
		return EXIT_FAILURE;
	}

	m_MouseLastX = WindowMgr->WINDOW_WIDTH / 2; 
	m_MouseLastY = WindowMgr->WINDOW_HEIGHT / 2;

	// glfwSetInputMode(WindowMgr->GetWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(WindowMgr->GetWindowPointer(), this);
	glfwSetCursorPosCallback(WindowMgr->GetWindowPointer(), SceneManager::MouseCallbackDispatcher);
	glfwSetScrollCallback(WindowMgr->GetWindowPointer(), ScrollCallbackDispatcher);
	glfwSetJoystickCallback(SceneManager::JoystickCallback);
    
	spdlog::info("Initialized project.");

	// spdlog::info("Initialized ImGui.");

	InitializeUI();

	MainCamera = make_shared<Camera>(glm::vec3(0.f, 25.f, 47.f), glm::vec3(0.0, 1.0, 0.0), -90.f, -25.f);

	Physics = &engine.GetPhysicsEngine();
	// physics = new PhysicsEngine();
	// physics->Init();

	LoadModels();
	AssignSceneGraph();

	glEnable(GL_DEPTH_TEST);

	m_Skybox = Skybox(*AssetMgr->SkyboxShader);
    
	//m_SkyboxShader.setInt("skybox", 0);

	m_WorldParent.UpdateSelfAndChild();

	glm::vec3 floorHalfExtents = glm::vec3(FLOOR_SCALE / 2.0f, FLOOR_SCALE / 2.0f, 1.0f);
	m_Floor.AddComponent<RigidBody>();
	m_Floor.GetComponent<RigidBody>()->Init(floorHalfExtents, true);


	//GAMEPAD

	inputManager = InputManager();

	inputManager.createAction("MoveForward");
	inputManager.createAction("MoveStrafe");

	inputManager.addBinding("MoveForward", {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_Y });
	inputManager.addBinding("MoveStrafe", {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_X });

	p1 = new Player(inputManager, m_SpawnManager, *AssetMgr->BasicShader, 0);
	p2 = new Player(inputManager, m_SpawnManager, *AssetMgr->BasicShader, 1);

	objectsTransform.AddChild(p1->body.get());
	objectsTransform.AddChild(p2->body.get());
	p1->body->transform->Position = glm::vec3(0.0f, 5.0f, 0.0f);
	p2->body->transform->Position = glm::vec3(5.0f, 5.0f, 0.0f);
}

void SceneManager::UpdateScene()
{
	input(WindowMgr->GetWindowPointer());
}


void SceneManager::RenderScene()
{
    glm::mat4 skyboxView = glm::mat4(glm::mat3(MainCamera->GetViewMatrix()));
    glm::mat4 view = MainCamera->GetViewMatrix();
	
    glm::mat4 projection = MainCamera->GetProjectionMatrix(float(WindowMgr->WINDOW_WIDTH) / float(WindowMgr->WINDOW_HEIGHT), CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);

    m_Skybox.DrawSkybox(skyboxView, projection);

    AssetMgr->BasicShader->Use();
    AssetMgr->BasicShader->SetMat4("view", view);
    AssetMgr->BasicShader->SetMat4("projection", projection);

	AssetMgr->BasicShader->SetBool("useDirLight", false);
	AssetMgr->BasicShader->SetBool("usePointLight", false);
	AssetMgr->BasicShader->SetBool("useSpotLight1", false);
	
    UpdateShaderLight(&m_WorldParent, *AssetMgr->BasicShader);

    AssetMgr->UIShader->Use();
    AssetMgr->UIShader->SetMat4("projection", projection);
    
    AssetMgr->SliderShader->Use();
    AssetMgr->SliderShader->SetMat4("projection", projection);
    AssetMgr->SliderShader->SetFloat("slider_load", m_CurrentRotationDegrees / 360.0f);

    m_WorldParent.transform->Position = glm::vec3(0.f, 0.f, -30.f);
    m_WorldParent.transform->Scale = glm::vec3(0.7f);

    duckTransparent.transform->Scale = glm::vec3(0.1f, 0.1f, 0.1f);
    duckTransparent.transform->Position = glm::vec3(16.4f, 13.5f, 0.f);
    duckTransparent.transform->EulerAngles.x = 90.f;

    slider.transform->Scale = glm::vec3(0.2f, 0.1f, 0.02f);
    slider.transform->Position = glm::vec3(-10.4f, 13.5f, 0.f);
    slider.transform->EulerAngles.x = 90.f;
    
    m_CurrentRotationDegrees += Time::GetDeltaTime() * 60.0f;

    if (static_cast<int>(m_CurrentRotationDegrees) / 360 >= 1)
    {
        m_CurrentRotationDegrees = 0;
        m_RotationCount++;
    }

    m_SpawnCounter += Time::GetDeltaTime();
    if (m_SpawnCounter >= m_SpawnTime)
    {
        m_SpawnCounter = 0;
        GameObject* spawnedEntity = m_SpawnManager.SpawnEntity(*AssetMgr->BasicShader);
        if (spawnedEntity != nullptr)
        {
            float posX = Random::GetRandomInt(-WALL_X_BORDER, WALL_X_BORDER);
            float posY = Random::GetRandomInt(-WALL_Y_BORDER, WALL_Y_BORDER);
			//spawnedEntity->transform->Scale = glm::vec3(2, 2, 2);
            spawnedEntity->transform->Position = glm::vec3(posX, 5, posY);
			

            spawnedEntity->UpdateSelfAndChild();
            spawnedEntity->AddComponent<RigidBody>();
            spawnedEntity->GetComponent<RigidBody>()->Init();

            objectsTransform.AddChild(spawnedEntity);
        }
    }

    monkey.transform->Position = glm::vec3(-5.f, 0.f, 0.f);
    monkey.transform->EulerAngles.x = m_CurrentRotationDegrees;
    monkey.transform->Scale = glm::vec3(2.f);

    m_Ball1.transform->Position = glm::vec3(0, 30.0f, -20.0f);

	inputManager.update();
	p1->Update(Time::GetDeltaTime());
	p2->Update(Time::GetDeltaTime());

    m_WorldParent.UpdateSelfAndChild();
    m_WorldParent.DrawSelfAndChild();

    glDisable(GL_DEPTH_TEST);

    if (Physics != nullptr)
    {
        Physics->DrawHitboxes(*AssetMgr->LineShader, view, projection);
    }


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::wstring monkeyText = L"FPS: " + std::to_wstring(Time::GetFPS());

    m_TextRenderer.RenderText(*AssetMgr->TextShader, monkeyText, 10.0f, 1700.0f, 2.0f, glm::vec3(0.3f, 0.3f, 0.3f));
    glEnable(GL_DEPTH_TEST);

	// Draw ImGui
	if (isDebugDraw)
	{
		DebugMgr->ImGUIBegin();
		DebugMgr->ImGUIRender(); // edit this function to add your own ImGui controls
		DebugMgr->ImGUIEnd();    // this call effectively renders ImGui
	}
}

void SceneManager::UpdateShaderLight(GameObject* gameObject, Shader& shader)
{
	if (gameObject == nullptr)
	{
		return;
	}
	
	LightSource* dLight = gameObject->GetComponent<DirectionalLight>();
	if (dLight != nullptr)
	{
		dLight->SetLightValues(shader);
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
		UpdateShaderLight(gameObject->Children[i], shader);
	}
}

//TODO remove those methods or move them elsewhere
void SceneManager::AssignSceneGraph()
{
	m_WorldParent.AddChild(&objectsTransform);

	m_WorldParent.AddChild(&m_LightSource);
	// m_LightSource.AddChild(&m_LightSourceObject);

	objectsTransform.AddChild(&duckTransparent);
	objectsTransform.AddChild(&slider);
	objectsTransform.AddChild(&monkey);
	objectsTransform.AddChild(&m_Ball1);

	AssignSceneModelsGraph();

	m_WorldParent.UpdateSelfAndChild();
}

void SceneManager::AssignSceneModelsGraph()
{
	m_WorldParent.AddChild(&m_Scene);
	m_Scene.AddChild(&m_Floor);
	m_Scene.AddChild(&m_WallDir);
	m_Scene.AddChild(&m_OnsenObjects);

	m_WallDir.AddChild(&m_WallBack);
	m_WallDir.AddChild(&m_WallFrontLeft);
	m_WallDir.AddChild(&m_WallFrontRight);
	m_WallDir.AddChild(&m_WallRight);
	m_WallDir.AddChild(&m_WallLeft);

	m_OnsenObjects.AddChild(&m_TowelsBed);

	m_Scene.transform->EulerAngles.x = -90.f;
	m_Floor.transform->EulerAngles.z = 90.f;

	m_WallBack.transform->Position.y = WALL_Y_BORDER;
	m_WallFrontLeft.transform->Position = glm::vec3(-63.f, -WALL_Y_BORDER, -30.f);
	m_WallFrontRight.transform->Position = glm::vec3(63.f, -WALL_Y_BORDER, -30.f);
	m_WallLeft.transform->EulerAngles.z = 90.f;
	m_WallRight.transform->EulerAngles.z = 90.f;
	m_WallLeft.transform->Position.x = -WALL_X_BORDER;
	m_WallRight.transform->Position.x = WALL_X_BORDER;

	m_OnsenObjects.transform->Position.z = 1.f;
}

void SceneManager::LoadSceneModels()
{
	m_Scene = GameObject();
	Model floorModel = Model(*AssetMgr->BasicShader, "res/models/scena_v1/floor/floor.fbx");
	m_Scene.AddComponent<Model>(*AssetMgr->BasicShader);
	m_Floor = GameObject();
	m_Floor.AddComponent<Model>(floorModel);
	m_FloorTex = *AssetMgr->GetTexture("res/models/scena_v1/floor/floor_textures/Stylized_Stone_Floor_010_basecolor.png");
	m_Floor.GetComponent<Model>()->AssignTexture(m_FloorTex);
	// m_Floor.AssignTexture(m_FloorTex);
	m_WallDir = GameObject();
	m_WallDir.AddComponent<Model>(*AssetMgr->BasicShader);
	Model wallModel = *AssetMgr->GetModel(*AssetMgr->BasicShader, "res/models/scena_v1/walls/wall1.fbx");
	m_WallBack = GameObject();
	m_WallBack.AddComponent<Model>(wallModel);
	m_WallFrontLeft = GameObject();
	m_WallFrontLeft.AddComponent<Model>(wallModel);
	m_WallFrontRight = GameObject();
	m_WallFrontRight.AddComponent<Model>(wallModel);
	m_WallLeft = GameObject();
	m_WallLeft.AddComponent<Model>(wallModel);
	m_WallRight = GameObject();
	m_WallRight.AddComponent<Model>(wallModel);
	Model towelBedModel = *AssetMgr->GetModel(*AssetMgr->BasicShader, "res/models/scena_v1/for_towels/for_towels.fbx");
	m_TowelsBed = GameObject();
	m_TowelsBed.AddComponent<Model>(towelBedModel);

	m_WallTex = *AssetMgr->GetTexture("res/models/scena_v1/walls/walls_textures/Stylized_Wall_002_basecolor.png");
	m_WallBack.GetComponent<Model>()->AssignTexture(m_WallTex);
	m_WallFrontLeft.GetComponent<Model>()->AssignTexture(m_WallTex);
	m_WallFrontRight.GetComponent<Model>()->AssignTexture(m_WallTex);
	m_WallLeft.GetComponent<Model>()->AssignTexture(m_WallTex);
	m_WallRight.GetComponent<Model>()->AssignTexture(m_WallTex);
}

void SceneManager::LoadModels()
{
	m_WorldParent = GameObject();
	objectsTransform = GameObject();
	// objectsTransform.AddComponent<Model>(m_BasicShader);

	duckTransparent = GameObject();
	Model UISprite = *AssetMgr->GetModel(*AssetMgr->UIShader, "res/models/house/floor.obj");
	duckTransparent.AddComponent<Model>(UISprite);
	m_UIDuckTex = *AssetMgr->GetTexture("res/textures/duck.png");
	duckTransparent.GetComponent<Model>()->AssignTexture(m_UIDuckTex);
	slider = GameObject();
	slider.AddComponent<Model>(UISprite);
	slider.GetComponent<Model>()->ReassignShader(*AssetMgr->SliderShader);
	m_SliderTex = *AssetMgr->GetTexture("res/textures/white.png");
	slider.GetComponent<Model>()->AssignTexture(m_SliderTex);
	//house_floor.ScaleTexture(FLOOR_TEX_SCALE * FLOOR_SCALE);
	monkey = GameObject();
	Model monkeyModel = *AssetMgr->GetModel(*AssetMgr->BasicShader, "res/models/monkey/Monkey.obj");
	monkey.AddComponent<Model>(monkeyModel);

	m_Ball1 = GameObject();
	Model ballModel = *AssetMgr->GetModel(*AssetMgr->BasicShader, "res/models/sphere/ball.obj");
	m_Ball1.AddComponent<Model>(ballModel);

	m_LightSourceObject = GameObject();
	Model lightModel = *AssetMgr->GetModel(*AssetMgr->LightSourceShader, "res/models/sphere/ball.obj");
	m_LightSourceObject.AddComponent<Model>(lightModel);
	
	m_LightSource = GameObject();
	m_LightSource.AddComponent<PointLight>(MainCamera, m_LightSource.transform, glm::vec3(1));
	m_LightSource.transform->Position = glm::vec3(0.f, 15.0f, -30.0f);

	LoadSceneModels();
}

void SceneManager::InitializeUI()
{
	glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(WindowMgr->WINDOW_WIDTH), 0.0f, static_cast<float>(WindowMgr->WINDOW_HEIGHT));
	AssetMgr->TextShader->Use();
	AssetMgr->TextShader->SetMat4("projection", textProjection);

	m_TextRenderer.Init((Loader::RelativePath()+ "res/fonts/Berylium.ttf").c_str(), 48);
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
		spdlog::info("Joystick connected: {}", glfwGetJoystickName(jid));
	}
	else if (event == GLFW_DISCONNECTED) {
		spdlog::warn("Joystick disconnected: {}", jid);
	}
}
