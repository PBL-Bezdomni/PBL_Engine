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
#include "Game/Scripts/Player.h"
#include "Engine/InputManager.h"
#include "Engine/Light/DirectionalLight.h"
#include "Engine/Light/LightSource.h"
#include "Engine/Light/PointLight.h"
#include "Engine/Light/SpotLight.h"
#include "Engine/Loader.h"

#include "Engine/JSONImporter.h"

#include "Game/Scripts/MassageTable.h"
#include "Game/Scripts/Animal.h"


#define _USE_MATH_DEFINES

void SceneManager::Initialize()
{
	Engine& engine = Engine::GetInstance();

	Physics = &engine.GetPhysicsEngine();
	WindowMgr = &engine.GetWindowManager();
	AssetMgr = &engine.GetAssetManager();

	if (!engine.GetWindowManager().GetIsInitialized())
	{
		spdlog::error("Failed to initialize project!");
		return;
	}
	
	m_JSONImporter = make_unique<JSONImporter>();
	m_JSONImporter->Initialize();
	
	// glfwSetInputMode(WindowMgr->GetWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(WindowMgr->GetWindowPointer(), this);
	glfwSetCursorPosCallback(WindowMgr->GetWindowPointer(), SceneManager::MouseCallbackDispatcher);
	glfwSetScrollCallback(WindowMgr->GetWindowPointer(), ScrollCallbackDispatcher);
	glfwSetJoystickCallback(SceneManager::JoystickCallback);
    
	spdlog::info("Initialized project.");

	std::string fontPath = Loader::RelativePath() + "res/fonts/Choko Regular_8337.otf";
	
	m_UIManager.Init(AssetMgr, WindowMgr, fontPath.c_str());
	
	//GAMEPAD

	inputManager = &engine.GetGameManager().GetInputManager();

	inputManager->createAction("MoveForward");
	inputManager->createAction("MoveStrafe");

	inputManager->addBinding("MoveForward", {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_Y });
	inputManager->addBinding("MoveStrafe", {BindingType::Axis, GLFW_GAMEPAD_AXIS_LEFT_X });
}

void SceneManager::LoadScene()
{
	m_MouseLastX = WindowMgr->WINDOW_WIDTH / 2; 
	m_MouseLastY = WindowMgr->WINDOW_HEIGHT / 2;
	
	MainCamera = make_shared<Camera>(glm::vec3(0.f, 25.f, 47.f), glm::vec3(0.0, 1.0, 0.0), -90.f, -25.f);
	MainCamera->SetAspect(float(WindowMgr->WINDOW_WIDTH) / float(WindowMgr->WINDOW_HEIGHT));
	
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
	m_Player1.transform->Position = glm::vec3(0.0f, 1.0f, 0.0f);
	m_Player2.transform->Position = glm::vec3(5.0f, 5.0f, 0.0f);

	m_WorldParent.UpdateSelfAndChild();

	m_Player1.AddComponent<RigidBody>();
	m_Player1.GetComponent<RigidBody>()->Init(glm::vec3(1.0f, 1.0f, 1.0f), false);

	m_Player2.AddComponent<RigidBody>();
	m_Player2.GetComponent<RigidBody>()->Init(glm::vec3(1.0f, 1.0f, 1.0f), false);


	// SHADOW


	AssetMgr->BasicShader->Use();
	AssetMgr->BasicShader->SetInt("shadowMap", 20);
}

void SceneManager::UpdateScene()
{
	input(WindowMgr->GetWindowPointer());
}


void SceneManager::RenderScene()
{
	glEnable(GL_DEPTH_TEST);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(MainCamera->GetViewMatrix()));
    glm::mat4 view = MainCamera->GetViewMatrix();
    glm::mat4 projection = MainCamera->GetProjectionMatrix();

    AssetMgr->BasicShader->Use();
    AssetMgr->BasicShader->SetMat4("view", view);
    AssetMgr->BasicShader->SetMat4("projection", projection);

	AssetMgr->BasicShader->SetBool("useDirLight", false);
	AssetMgr->BasicShader->SetBool("usePointLight", false);
	AssetMgr->BasicShader->SetBool("useSpotLight1", false);

	if (AssetMgr->PieChartShader != nullptr)
	{
		AssetMgr->PieChartShader->Use();
		AssetMgr->PieChartShader->SetMat4("view", view);
		AssetMgr->PieChartShader->SetMat4("projection", projection);

		AssetMgr->BasicShader->Use();
	}
	
    UpdateShaderLight(&m_WorldParent, *AssetMgr->BasicShader, *AssetMgr->SimpleDepthShader);

    m_WorldParent.UpdateSelfAndChild();

	int windowH, windowW;
	glfwGetWindowSize(WindowMgr->GetWindowPointer(), &windowW, &windowH);
	glViewport(0, 0, windowW, windowH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_WorldParent.DrawSelfAndChild(NULL);

	if (AssetMgr->PieChartShader != nullptr)
	{
		AssetMgr->PieChartShader->Use();
		AssetMgr->PieChartShader->SetMat4("view", view);
		AssetMgr->PieChartShader->SetMat4("projection", projection);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthMask(GL_FALSE);

		for (Animal* animal : m_AnimalsList)
		{
			if (animal != nullptr && animal->GetIndicatorObject() != nullptr)
			{
				glm::vec3 worldPos = glm::vec3(animal->GetOwner()->transform->ModelMatrix[3]);
				animal->GetIndicatorObject()->transform->Position = worldPos + glm::vec3(0.0f, -0.5f, 0.0f);
				animal->GetIndicatorObject()->UpdateSelfAndChild();

				animal->UpdateIndicatorColors();
				animal->GetIndicatorObject()->DrawSelfAndChild(NULL);
			}
		}

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		AssetMgr->BasicShader->Use();
	}

	m_Skybox.DrawSkybox(skyboxView, projection);

    glDisable(GL_DEPTH_TEST);

    if (Physics != nullptr)
    {
        Physics->DrawHitboxes(*AssetMgr->LineShader, view, projection);
    }

	// IMPORTANT: Do not write things below Freetype/UI, if you do not know what you are doing, thanks :)
	// Draw UI
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

	m_MoneyPanel.Text = L"110";
	m_FpsPanel.Text = L"FPS: " + std::to_wstring(Time::GetFPS());

	m_UIManager.DrawPanelWithText(*AssetMgr->UIShader, *AssetMgr->TextShader, m_MoneyPanel);
	m_UIManager.DrawPanelWithText(*AssetMgr->UIShader, *AssetMgr->TextShader, m_TimerPanel);
	m_UIManager.DrawPanelWithText(*AssetMgr->UIShader, *AssetMgr->TextShader, m_FpsPanel);
}

void SceneManager::AddAnimal(shared_ptr<GameObject> spawnedEntity)
{
	if (spawnedEntity != nullptr)
	{
		m_WorldParent.AddChild(spawnedEntity.get());

		Animal* animal = spawnedEntity->GetComponent<Animal>();
		if (animal != nullptr)
		{
			m_AnimalsList.push_back(animal);
		}
	}
}

shared_ptr<GameObject> SceneManager::Instantiate(string path, shared_ptr<Shader> shader)
{
	// TODO make load components data from prefab path instead of passing model path
	shared_ptr<GameObject> go = make_shared<GameObject>();
	if (shader == nullptr)
	{
		shader = AssetMgr->BasicShader;
	}
	Model model = *AssetMgr->GetModel(*shader, path.c_str());
	go->AddComponent<Model>(model);
	m_GameObjects.push_back(go);
	return m_GameObjects.back();
}

shared_ptr<Camera> SceneManager::GetMainCamera()
{
	return MainCamera;
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
		unsigned int depthMap = 0;
		depthMap = dLight->getShadowMap(m_WorldParent, depthShader);
		lightSpaceMatrix = dLight->getLightViewMatrix();
		AssetMgr->BasicShader->Use();

		AssetMgr->BasicShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_2D, depthMap);
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

	m_WorldParent.transform->Position = glm::vec3(0.f, 0.f, -30.f);
	m_WorldParent.transform->Scale = glm::vec3(0.7f);

	m_WorldParent.UpdateSelfAndChild();
}

void SceneManager::LoadModels()
{
	m_WorldParent = GameObject();
	vector<shared_ptr<GameObject>> objs = m_JSONImporter->ImportScene("scene", &m_WorldParent);
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
}

void SceneManager::InitializeUI()
{
	glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(WindowMgr->WINDOW_WIDTH), static_cast<float>(WindowMgr->WINDOW_HEIGHT), 0.0f);
	AssetMgr->TextShader->Use();
	AssetMgr->TextShader->SetMat4("projection", textProjection);

	m_MoneyPanel.TextureID = m_UIPanelTex.ID;
	m_MoneyPanel.Size = glm::vec2(300.0f, 100.0f);
	m_MoneyPanel.Position = glm::vec2(10.0f, 10.0f);
	m_MoneyPanel.TextColor = glm::vec3(0.333f, 0.227f, 0.196f);
	m_MoneyPanel.HasIcon = true;
	m_MoneyPanel.InconTextureID = m_UICoinTex.ID;
	m_MoneyPanel.IconSize = glm::vec2(40.0f, 40.0f);

	m_TimerPanel.TextureID = m_UIPanelTex.ID;
	m_TimerPanel.Size = glm::vec2(300.0f, 100.0f);
	m_TimerPanel.Position = glm::vec2((WindowMgr->WINDOW_WIDTH / 2.0f) - 150.0f, 10.0f);
	m_TimerPanel.TextColor = glm::vec3(0.333f, 0.227f, 0.196f);

	m_FpsPanel.HasTexture = false;
	m_FpsPanel.Size = glm::vec2(300.0f, 100.0f);
	m_FpsPanel.Position = glm::vec2((WindowMgr->WINDOW_WIDTH) - 300.0f, 10.0f);
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
