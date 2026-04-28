#include <stdio.h>
#include <random>
#include <string.h>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <vector>
#include "miniaudio.h"
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

#include "FreeType.h"
#include "Shader.h"
#include "Model.h"
#include "GameObject.h"
#include "Quaternion.h"
#include "Camera.h"
#include "Skybox.h"
#include "Engine/Loader.h"
#include "Engine/Engine.h"
#include "SpawnManager.h"
#include "Engine/DebugManager.h"
#include "Engine/Time.h"
#include "Model.h"
#include "Engine/PhysicsEngine/PhysicsEngine.h"
#include "Engine/Components/RigidBody.h"

#define _USE_MATH_DEFINES

const float CAMERA_NEAR_PLANE = .1f;
const float CAMERA_FAR_PLANE = 200.f;
const float FLOOR_TEX_SCALE = 8.f;
const float FLOOR_SCALE = 100.f;
const int HOUSE_NET_DIM = 200;

float WALL_X_BORDER = 37.f;
float WALL_Y_BORDER = 50.f;

const float FIXED_TIME_STEP = 1.0f / 60.0f;
float physicsAccumulator = 0.0f;

void LoadTexture(const char* path, Texture* texture);
void LoadModels();
void AssignSceneGraph();

void init_shader();

void input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int RandomValue(int min, int max);
void render();

void SetupShaderLight(Shader& shader);

void LoadSceneModels();
void AssignSceneModelsGraph();

Camera MainCamera;

// Mouse fields
float m_MouseLastX;// = WINDOW_WIDTH / 2; 
float m_MouseLastY;// = WINDOW_HEIGHT / 2;
bool m_IsFirstMouse = true;

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

GameObject world;
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

unsigned int m_SkyboxVAO;
unsigned int m_SkyboxVBO;

// Values for light
bool m_UseDirLight = false;
bool m_UsePointLight = true;
bool m_UseSpotLight1 = false;
bool m_UseSpotLight2 = false;

glm::vec3 m_PointLightPos = glm::vec3(0.f, 15.0f, -30.0f);
glm::vec3 m_PointLightColor = glm::vec3(1.f, 1.f, 1.f);
float m_PointLightRadius = 100.f;
float m_PointLightHeight = 5.f;
float m_PointLightLinear = 0.0014f;
float m_PointLightQuadratic = 0.000007f;

glm::vec3 m_DirLightDirection = glm::vec3(0.f, -1.0f, 0.f);
glm::vec3 m_DirLightVisualPos = glm::vec3(0.f, 10.f, 0.f);
glm::vec3 m_DirLightColor = glm::vec3(1.f, 1.f, 1.f);
float m_DirLightAngleX = -45.f;
float m_DirLightAngleZ = 30.f;

glm::vec3 m_SpotLight1Color = glm::vec3(1.f, 1.f, 1.f);
glm::vec3 m_SpotLight1Position = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 m_SpotLight1Direction = glm::vec3(0.0f, 0.0f, 1.0f);
float m_SpotLight1CutOff = 24.5f;
float m_SpotLight1OuterCutOff = 30.5f;
float m_SpotLight1Angle = 0.f;
float m_SpotLight1Linear = 0.09f;
float m_SpotLight1Quadratic = 0.032f;

glm::vec3 m_SpotLight2Color = glm::vec3(1.f, 1.f, 1.f);
glm::vec3 m_SpotLight2Position = glm::vec3(8.f, 0.0f, -6.0f);
glm::vec3 m_SpotLight2Direction = glm::vec3(0.0f, 0.0f, -1.0f);
float m_SpotLight2CutOff = 8.5f;
float m_SpotLight2OuterCutOff = 12.5f;
float m_SpotLight2Angle = 135.f;
float m_SpotLight2Linear = 0.09f;
float m_SpotLight2Quadratic = 0.032f;

float m_Posx = 0;
float m_Posy = 0;
float m_Posz = 0;

// Loader LoadManager = Loader();
SpawnManager m_SpawnManager = SpawnManager();
Skybox m_Skybox;

WindowManager* WindowMgr = nullptr;
DebugManager* DebugMgr = nullptr;
PhysicsEngine* Physics = nullptr;
AssetManager* AssetMgr = nullptr;

int main(int, char**)
{
    Engine& engine = Engine::GetInstance();
    bool isDebugDraw = engine.GetIsDebugDrawn();

    WindowMgr = &engine.GetWindowManager();
    AssetMgr = &engine.GetAssetManager();
    
    // engine.Start();
    if (!engine.GetWindowManager().GetIsInitialized())
    {
        spdlog::error("Failed to initialize project!");
        return EXIT_FAILURE;
    }

    m_MouseLastX = WindowMgr->WINDOW_WIDTH / 2; 
    m_MouseLastY = WindowMgr->WINDOW_HEIGHT / 2;

    glfwSetInputMode(WindowMgr->GetWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(WindowMgr->GetWindowPointer(), mouse_callback);
    glfwSetScrollCallback(WindowMgr->GetWindowPointer(), scroll_callback);
    
    spdlog::info("Initialized project.");

    // spdlog::info("Initialized ImGui.");

    init_shader();

    MainCamera = Camera(glm::vec3(0.f, 25.f, 47.f), glm::vec3(0.0, 1.0, 0.0), -90.f, -25.f);

    Physics = &engine.GetPhysicsEngine();
    // physics = new PhysicsEngine();
    // physics->Init();

    LoadModels();
    AssignSceneGraph();

    glEnable(GL_DEPTH_TEST);

    m_Skybox = Skybox(m_SkyboxShader);
    
    //m_SkyboxShader.setInt("skybox", 0);

    world.UpdateSelfAndChild();

    glm::vec3 floorHalfExtents = glm::vec3(FLOOR_SCALE / 2.0f, FLOOR_SCALE / 2.0f, 1.0f);
    m_Floor.AddComponent<RigidBody>();
    m_Floor.GetComponent<RigidBody>()->Init(floorHalfExtents, true);
    
    // Main loop
    while (!glfwWindowShouldClose(WindowMgr->GetWindowPointer()))
    {
        // Clear screen
        WindowMgr->ClearFrame();

        // Process I/O operations here
        input(WindowMgr->GetWindowPointer());

        // Update game objects' state here
        Time::Update();

        float frameTime = Time::GetDeltaTime();
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        physicsAccumulator += frameTime;

        while (physicsAccumulator >= FIXED_TIME_STEP)
        {
            Physics->Update(FIXED_TIME_STEP);
            physicsAccumulator -= FIXED_TIME_STEP;
        }

        // OpenGL rendering code here
        render();

        // Draw ImGui
        if (isDebugDraw)
        {
            DebugMgr->ImGUIBegin();
            DebugMgr->ImGUIRender(); // edit this function to add your own ImGui controls
            DebugMgr->ImGUIEnd();    // this call effectively renders ImGui
        }


        // End frame and swap buffers (double buffering)
        WindowMgr->EndFrame();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_BasicShader.DeleteProgram();
    //glDeleteProgram(m_ShaderProgram);

    glfwDestroyWindow(WindowMgr->GetWindowPointer());
    glfwTerminate();

    return 0;
}

void init_shader()
{
    m_BasicShader = *AssetMgr->GetShader("res/shaders/basic.vert", "res/shaders/basic.frag");
    m_UIShader = *AssetMgr->GetShader("res/shaders/UIShader.vert", "res/shaders/UIShader.frag");
    m_SliderShader = *AssetMgr->GetShader("res/shaders/UIShader.vert", "res/shaders/UISlider.frag");
    m_SkyboxShader = *AssetMgr->GetShader("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");
    m_LineShader = *AssetMgr->GetShader("res/shaders/line.vert", "res/shaders/line.frag");

    LoadTexture((Loader::RelativePath()+ "res/textures/duck.png").c_str(), &m_UIDuckTex);
    LoadTexture((Loader::RelativePath()+ "res/models/scena_v1/floor/floor_textures/Stylized_Stone_Floor_010_basecolor.png").c_str(), &m_FloorTex);
    LoadTexture((Loader::RelativePath()+ "res/models/scena_v1/walls/walls_textures/Stylized_Wall_002_basecolor.png").c_str(), &m_WallTex);
    // LoadTexture((Loader::RelativePath()+ "res/models/scena_v1/for_towels/drewno.jpg").c_str(), &m_TowelsTex);
    LoadTexture((Loader::RelativePath()+ "res/textures/white.png").c_str(), &m_SliderTex);
    
    m_TextShader = *AssetMgr->GetShader("res/shaders/text.vert", "res/shaders/text.frag");

    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(WindowMgr->WINDOW_WIDTH), 0.0f, static_cast<float>(WindowMgr->WINDOW_HEIGHT));
    m_TextShader.Use();
    m_TextShader.SetMat4("projection", textProjection);

    m_TextRenderer.Init((Loader::RelativePath()+ "res/fonts/Berylium.ttf").c_str(), 48);
}

void input(GLFWwindow* window)
{
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(FORWARD, Time::GetDeltaTime());
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(BACKWARD, Time::GetDeltaTime());
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(LEFT, Time::GetDeltaTime());
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(RIGHT, Time::GetDeltaTime());
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
            MainCamera.ProcessMouseMovement(0, 1.f);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            MainCamera.ProcessMouseMovement(0, -1.f);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            MainCamera.ProcessMouseMovement(1.f, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            MainCamera.ProcessMouseMovement(-1.f, 0);
        }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

        MainCamera.ProcessMouseMovement(xoffset, yoffset);
    }
    else
    {
        m_IsFirstMouse = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        MainCamera.ProcessMouseScroll(yoffset);
    }
}

float m_CurrentRotationDegrees;
int m_RotationCount;
float m_SpawnCounter;
float m_SpawnTime = 2.f;

int RandomValue(int min, int max)
{
    max = abs(min) + abs(max);
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0,max);
    int r = dist(rng);
    r -= abs(min);
    return r;
}

void render()
{
    glm::vec3 axisX = glm::vec3(1.0f, 0.f, 0.f);
    glm::vec3 axisY = glm::vec3(.0f, 1.f, 0.f);
    glm::vec3 axisZ = glm::vec3(.0f, 0.f, 1.f);
    Quaternion quat = Quaternion();
    
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(MainCamera.GetViewMatrix()));;
    // note that we're translating the scene in the reverse direction of where we want to move
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = MainCamera.GetViewMatrix();
   
    glm::mat4 projection;
    projection = MainCamera.GetProjectionMatrix(float(WindowMgr->WINDOW_WIDTH) / float(WindowMgr->WINDOW_HEIGHT), CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);

    m_Skybox.DrawSkybox(skyboxView, projection);

    m_BasicShader.Use();
    m_BasicShader.SetMat4("view", view);
    m_BasicShader.SetMat4("projection", projection);
    SetupShaderLight(m_BasicShader);

    m_UIShader.Use();
    m_UIShader.SetMat4("projection", projection);
    
    m_SliderShader.Use();
    m_SliderShader.SetMat4("projection", projection);
    m_SliderShader.SetFloat("slider_load", m_CurrentRotationDegrees / 360.0f);

    world.transform->Position = glm::vec3(0.f, 0.f, -30.f);
    world.transform->Scale = glm::vec3(0.7f);

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
        GameObject* spawnedEntity = m_SpawnManager.SpawnEntity(m_BasicShader);
        if (spawnedEntity != nullptr)
        {
            float posX = RandomValue(-WALL_X_BORDER, WALL_X_BORDER);
            float posY = RandomValue(-WALL_Y_BORDER, WALL_Y_BORDER);
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

    // m_PointLightPos = quat.RotateQuaternion(glm::vec3(m_PointLightRadius, m_PointLightHeight, 0.f), axisY, glfwGetTime() * 50);

    world.UpdateSelfAndChild();
    world.DrawSelfAndChild();

    glDisable(GL_DEPTH_TEST);

    if (Physics != nullptr)
    {
        Physics->DrawHitboxes(m_LineShader, view, projection);
    }


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::wstring monkeyText = L"FPS: " + std::to_wstring(Time::GetFPS());

    m_TextRenderer.RenderText(m_TextShader, monkeyText, 10.0f, 1700.0f, 2.0f, glm::vec3(0.3f, 0.3f, 0.3f));
    glEnable(GL_DEPTH_TEST);
}

void LoadTexture(const char* path, Texture* tex)
{
    tex->Path = path;
    tex->Type = "texture_diffuse";
    glGenTextures(1, &tex->ID);
    glBindTexture(GL_TEXTURE_2D, tex->ID);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //stbi_set_flip_vertically_on_load(1);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 4)
        {
            format = GL_RGBA;
        }
        spdlog::info("Loaded Texture: {} with {} channels", path, nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        spdlog::error("Failed to load texture");
        spdlog::error(stbi_failure_reason());
    }
    stbi_image_free(data);
}

void LoadModels()
{
    world = GameObject();
    objectsTransform = GameObject();
    objectsTransform.AddComponent<Model>(m_BasicShader);

    duckTransparent = GameObject();
    duckTransparent.AddComponent<Model>(m_UIShader, (Loader::RelativePath()+ "res/models/house/floor.obj").c_str());
    slider = GameObject();
    slider.AddComponent<Model>(m_SliderShader, (Loader::RelativePath()+ "res/models/house/floor.obj").c_str());
    duckTransparent.GetComponent<Model>()->AssignTexture(m_UIDuckTex);
    slider.GetComponent<Model>()->AssignTexture(m_SliderTex);
    //house_floor.ScaleTexture(FLOOR_TEX_SCALE * FLOOR_SCALE);
    monkey = GameObject();
    monkey.AddComponent<Model>(m_BasicShader, (Loader::RelativePath()+ "res/models/monkey/Monkey.obj").c_str());

    m_Ball1 = GameObject();
    m_Ball1.AddComponent<Model>(m_BasicShader, (Loader::RelativePath() + "res/models/sphere/ball.obj").c_str());

    LoadSceneModels();
}

void LoadSceneModels()
{
    m_Scene = GameObject();
    Model m = Model(m_BasicShader, (Loader::RelativePath()+ "res/models/scena_v1/floor/floor.fbx").c_str());
    m_Scene.AddComponent<Model>(m_BasicShader);
    m_Floor = GameObject();
    m_Floor.AddComponent<Model>(m);
    m_Floor.GetComponent<Model>()->AssignTexture(m_FloorTex);
    // m_Floor.AssignTexture(m_FloorTex);
    m_WallDir = GameObject();
    m_WallDir.AddComponent<Model>(m_BasicShader);
    m_WallBack = GameObject();
    m_WallBack.AddComponent<Model>(m_BasicShader, (Loader::RelativePath()+ "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallFrontLeft = GameObject();
    m_WallFrontLeft.AddComponent<Model>(m_BasicShader, (Loader::RelativePath()+ "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallFrontRight = GameObject();
    m_WallFrontRight.AddComponent<Model>(m_BasicShader, (Loader::RelativePath()+ "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallLeft = GameObject();
    m_WallLeft.AddComponent<Model>(m_BasicShader, (Loader::RelativePath()+ "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallRight = GameObject();
    m_WallRight.AddComponent<Model>(m_BasicShader, (Loader::RelativePath()+ "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_TowelsBed = GameObject();
    m_TowelsBed.AddComponent<Model>(m_BasicShader, (Loader::RelativePath()+ "res/models/scena_v1/for_towels/for_towels.fbx").c_str());
    
    m_WallBack.GetComponent<Model>()->AssignTexture(m_WallTex);
    m_WallFrontLeft.GetComponent<Model>()->AssignTexture(m_WallTex);
    m_WallFrontRight.GetComponent<Model>()->AssignTexture(m_WallTex);
    m_WallLeft.GetComponent<Model>()->AssignTexture(m_WallTex);
    m_WallRight.GetComponent<Model>()->AssignTexture(m_WallTex);
}

void AssignSceneGraph()
{
    world.AddChild(&objectsTransform);

    objectsTransform.AddChild(&duckTransparent);
    objectsTransform.AddChild(&slider);
    objectsTransform.AddChild(&monkey);
    objectsTransform.AddChild(&m_Ball1);

    AssignSceneModelsGraph();

    world.UpdateSelfAndChild();
}

void AssignSceneModelsGraph()
{
    world.AddChild(&m_Scene);
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

void SetupShaderLight(Shader& shader)
{
    shader.SetBool("useDirLight", m_UseDirLight);
    shader.SetVec3("dirLight.color", m_DirLightColor);
    shader.SetVec3("dirLight.direction", m_DirLightDirection);

    shader.SetBool("usePointLight", m_UsePointLight);
    shader.SetVec3("pointLight.color", m_PointLightColor);
    shader.SetVec3("pointLight.position", m_PointLightPos);
    shader.SetFloat("pointLight.constant", 1.0f);
    shader.SetFloat("pointLight.linear", m_PointLightLinear);
    shader.SetFloat("pointLight.quadratic", m_PointLightQuadratic);
    shader.SetVec3("viewPos", MainCamera.Position);

    shader.SetBool("useSpotLight1", m_UseSpotLight1);
    shader.SetVec3("spotLight1.color", m_SpotLight1Color);
    shader.SetVec3("spotLight1.position", m_SpotLight1Position);
    shader.SetVec3("spotLight1.direction", m_SpotLight1Direction);
    shader.SetFloat("spotLight1.cutOff", glm::cos(glm::radians(m_SpotLight1CutOff)));
    shader.SetFloat("spotLight1.outerCutOff", glm::cos(glm::radians(m_SpotLight1OuterCutOff)));
    shader.SetFloat("spotLight1.constant", 1.0f);
    shader.SetFloat("spotLight1.linear", m_SpotLight1Linear);
    shader.SetFloat("spotLight1.quadratic", m_SpotLight2Quadratic);

    shader.SetBool("useSpotLight2", m_UseSpotLight2);
    shader.SetVec3("spotLight2.color", m_SpotLight2Color);
    shader.SetVec3("spotLight2.position", m_SpotLight2Position);
    shader.SetVec3("spotLight2.direction", m_SpotLight2Direction);
    shader.SetFloat("spotLight2.cutOff", glm::cos(glm::radians(m_SpotLight2CutOff)));
    shader.SetFloat("spotLight2.outerCutOff", glm::cos(glm::radians(m_SpotLight2OuterCutOff)));
    shader.SetFloat("spotLight2.constant", 1.0f);
    shader.SetFloat("spotLight2.linear", m_SpotLight2Linear);
    shader.SetFloat("spotLight2.quadratic", m_SpotLight2Quadratic);
}
