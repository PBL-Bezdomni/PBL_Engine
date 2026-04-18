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
#include "Entity.h"
#include "Quaternion.h"
#include "Camera.h"
#include "Engine/Loader.h"
#include "Engine/Engine.h"
#include "SpawnManager.h"
#include "Engine/DebugManager.h"

#define _USE_MATH_DEFINES

const float CAMERA_NEAR_PLANE = .1f;
const float CAMERA_FAR_PLANE = 200.f;
const float FLOOR_TEX_SCALE = 8.f;
const float FLOOR_SCALE = 100.f;
const int HOUSE_NET_DIM = 200;

float WALL_X_BORDER = 37.f;
float WALL_Y_BORDER = 50.f;

void LoadTexture(const char* path, Texture* texture);
void LoadModels();
void AssignSceneGraph();

void init_shader();

void input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void update();
int RandomValue(int min, int max);
void render();

void GenerateSkybox();
void DrawSkybox(glm::mat4 view, glm::mat4 projection);

void SetupShaderLight(Shader& shader);

void LoadCubemapTextures(vector<string> faces, Texture* texture);

void LoadSceneModels();
void AssignSceneModelsGraph();

ImVec4 m_ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

Camera MainCamera;

float m_DeltaTime = 0.0f;
float m_LastFrame = 0.0f;

// Mouse fields
float m_MouseLastX;// = WINDOW_WIDTH / 2; 
float m_MouseLastY;// = WINDOW_HEIGHT / 2;
bool m_IsFirstMouse = true;

Shader m_BasicShader;
Shader m_SkyboxShader;
Shader m_UIShader;
Shader m_TextShader;
Shader m_SliderShader;

FreeType m_TextRenderer;

Texture m_FloorTex;
Texture m_WallTex;
Texture m_TableTex;
Texture m_WaterpoolTex;
Texture m_TowelsTex;
Texture m_SliderTex;
Texture m_SkyboxTex;
Texture m_UIDuckTex;

unsigned int m_SkyboxTexID;
Entity world;
Entity skybox;

Entity monkey;
Entity duckTransparent;
Entity slider;
Entity objectsTransform;

Entity m_Scene;
Entity m_Floor;
Entity m_WallDir;
Entity m_WallLeft;
Entity m_WallRight;
Entity m_WallBack;
Entity m_WallFrontRight;
Entity m_WallFrontLeft;
Entity m_OnsenObjects;
Entity m_TowelsBed;
Entity m_Waterpool;
Entity m_TablesDir;
Entity m_Table1;
Entity m_Table2;
Entity m_Table3;
Entity m_Table4;
Entity m_Table5;

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

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float m_Posx = 0;
float m_Posy = 0;
float m_Posz = 0;

Loader LoadManager = Loader();
SpawnManager m_SpawnManager = SpawnManager();

vector<string> m_CubemapFaces { 
    LoadManager.RelativePath + "res/textures/cubemap/right.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/left.jpg",
    LoadManager.RelativePath + "res/textures/cubemap/top.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/bottom.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/front.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/back.jpg"
};

WindowManager* WindowMgr = nullptr;
DebugManager* DebugMgr = nullptr;

int main(int, char**)
{
    Engine& engine = Engine::GetInstance();
    bool isDebugDraw = engine.GetIsDebugDrawn();

    WindowMgr = &Engine::GetInstance().GetWindowManager();
    
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

    LoadModels();
    AssignSceneGraph();

    glEnable(GL_DEPTH_TEST);

    GenerateSkybox();
    m_SkyboxShader.Use();
    m_SkyboxShader.SetInt("skybox", 0);
    
    //m_SkyboxShader.setInt("skybox", 0);
    
    // Main loop
    while (!glfwWindowShouldClose(WindowMgr->GetWindowPointer()))
    {
        // Clear screen
        WindowMgr->ClearFrame();

        // Process I/O operations here
        input(WindowMgr->GetWindowPointer());

        // Update game objects' state here
        update();

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
    m_BasicShader = Shader((LoadManager.RelativePath + "res/shaders/basic.vert").c_str(), (LoadManager.RelativePath + "res/shaders/basic.frag").c_str());
    m_UIShader = Shader((LoadManager.RelativePath + "res/shaders/UIShader.vert").c_str(), (LoadManager.RelativePath + "res/shaders/UIShader.frag").c_str());
    m_SliderShader = Shader((LoadManager.RelativePath + "res/shaders/UIShader.vert").c_str(), (LoadManager.RelativePath + "res/shaders/UISlider.frag").c_str());
    m_SkyboxShader = Shader((LoadManager.RelativePath + "res/shaders/cubemap.vert").c_str(), (LoadManager.RelativePath + "res/shaders/cubemap.frag").c_str());

    LoadTexture((LoadManager.RelativePath + "res/textures/duck.png").c_str(), &m_UIDuckTex);
    LoadTexture((LoadManager.RelativePath + "res/models/scena_v1/floor/floor_textures/Stylized_Stone_Floor_010_basecolor.png").c_str(), &m_FloorTex);
    LoadTexture((LoadManager.RelativePath + "res/models/scena_v1/walls/walls_textures/Stylized_Wall_002_basecolor.png").c_str(), &m_WallTex);
    // LoadTexture((LoadManager.RelativePath + "res/models/scena_v1/for_towels/drewno.jpg").c_str(), &m_TowelsTex);
    LoadTexture((LoadManager.RelativePath + "res/textures/white.png").c_str(), &m_SliderTex);
    
    m_TextShader = Shader((LoadManager.RelativePath + "res/shaders/text.vert").c_str(), (LoadManager.RelativePath + "res/shaders/text.frag").c_str());

    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(WindowMgr->WINDOW_WIDTH), 0.0f, static_cast<float>(WindowMgr->WINDOW_HEIGHT));
    m_TextShader.Use();
    m_TextShader.SetMat4("projection", textProjection);

    m_TextRenderer.Init((LoadManager.RelativePath + "res/fonts/Berylium.ttf").c_str(), 48);
}

void input(GLFWwindow* window)
{
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(FORWARD, m_DeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(BACKWARD, m_DeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(LEFT, m_DeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            MainCamera.ProcessKeyboard(RIGHT, m_DeltaTime);
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

void update()
{
    float currentFrame = glfwGetTime();
    m_DeltaTime = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;
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

    DrawSkybox(skyboxView, projection);

    m_BasicShader.Use();
    m_BasicShader.SetMat4("view", view);
    m_BasicShader.SetMat4("projection", projection);
    SetupShaderLight(m_BasicShader);

    m_UIShader.Use();
    m_UIShader.SetMat4("projection", projection);
    
    m_SliderShader.Use();
    m_SliderShader.SetMat4("projection", projection);
    m_SliderShader.SetFloat("slider_load", m_CurrentRotationDegrees / 360.0f);

    world.transform.Position = glm::vec3(0.f, 0.f, -30.f);
    world.transform.Scale = glm::vec3(0.7f);

    duckTransparent.transform.Scale = glm::vec3(0.1f, 0.1f, 0.1f);
    duckTransparent.transform.Position = glm::vec3(16.4f, 13.5f, 0.f);
    duckTransparent.transform.EulerAngles.x = 90.f;

    slider.transform.Scale = glm::vec3(0.2f, 0.1f, 0.02f);
    slider.transform.Position = glm::vec3(-10.4f, 13.5f, 0.f);
    slider.transform.EulerAngles.x = 90.f;
    
    m_CurrentRotationDegrees += m_DeltaTime * 60.0f;

    if (static_cast<int>(m_CurrentRotationDegrees) / 360 >= 1)
    {
        m_CurrentRotationDegrees = 0;
        m_RotationCount++;
    }

    m_SpawnCounter += m_DeltaTime;
    if (m_SpawnCounter >= m_SpawnTime)
    {
        m_SpawnCounter = 0;
        Entity* spawnedEntity = m_SpawnManager.SpawnEntity(m_BasicShader, LoadManager);
        if (spawnedEntity != nullptr)
        {
            float posX = RandomValue(-WALL_X_BORDER, WALL_X_BORDER);
            float posY = RandomValue(-WALL_Y_BORDER, WALL_Y_BORDER);
            spawnedEntity->transform.Position = glm::vec3(posX, 5, posY);
            objectsTransform.AddChild(spawnedEntity);
        }
    }

    monkey.transform.Position = glm::vec3(-5.f, 0.f, 0.f);
    monkey.transform.EulerAngles.x = m_CurrentRotationDegrees;
    monkey.transform.Scale = glm::vec3(2.f);

    // m_PointLightPos = quat.RotateQuaternion(glm::vec3(m_PointLightRadius, m_PointLightHeight, 0.f), axisY, glfwGetTime() * 50);

    world.UpdateSelfAndChild();
    world.DrawSelfAndChild();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::wstring monkeyText = L"Monkey's rotations: " + std::to_wstring(m_RotationCount);

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
    world = Entity();
    objectsTransform = Entity(m_BasicShader);

    duckTransparent = Entity(m_UIShader, (LoadManager.RelativePath + "res/models/house/floor.obj").c_str());
    slider = Entity(m_SliderShader, (LoadManager.RelativePath + "res/models/house/floor.obj").c_str());
    duckTransparent.AssignTexture(m_UIDuckTex);
    slider.AssignTexture(m_SliderTex);
    //house_floor.ScaleTexture(FLOOR_TEX_SCALE * FLOOR_SCALE);
    monkey = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/monkey/Monkey.obj").c_str());

    LoadSceneModels();
}

void LoadSceneModels()
{
    m_Scene = Entity(m_BasicShader);
    m_Floor = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/scena_v1/floor/floor.fbx").c_str());
    m_Floor.AssignTexture(m_FloorTex);
    m_WallDir = Entity(m_BasicShader);
    m_WallBack = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallFrontLeft = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallFrontRight = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallLeft = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_WallRight = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/scena_v1/walls/wall1.fbx").c_str());
    m_TowelsBed = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/scena_v1/for_towels/for_towels.fbx").c_str());
    
    m_WallBack.AssignTexture(m_WallTex);
    m_WallFrontLeft.AssignTexture(m_WallTex);
    m_WallFrontRight.AssignTexture(m_WallTex);
    m_WallLeft.AssignTexture(m_WallTex);
    m_WallRight.AssignTexture(m_WallTex);
}

void AssignSceneGraph()
{
    world.AddChild(&objectsTransform);

    objectsTransform.AddChild(&duckTransparent);
    objectsTransform.AddChild(&slider);
    objectsTransform.AddChild(&monkey);

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

    m_Scene.transform.EulerAngles.x = -90.f;
    m_Floor.transform.EulerAngles.z = 90.f;

    m_WallBack.transform.Position.y = WALL_Y_BORDER;
    m_WallFrontLeft.transform.Position = glm::vec3(-63.f, -WALL_Y_BORDER, -30.f);
    m_WallFrontRight.transform.Position = glm::vec3(63.f, -WALL_Y_BORDER, -30.f);
    m_WallLeft.transform.EulerAngles.z = 90.f;
    m_WallRight.transform.EulerAngles.z = 90.f;
    m_WallLeft.transform.Position.x = -WALL_X_BORDER;
    m_WallRight.transform.Position.x = WALL_X_BORDER;

    m_OnsenObjects.transform.Position.z = 1.f;
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

void GenerateSkybox()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    m_SkyboxShader.Use();
    glGenVertexArrays(1, &m_SkyboxVAO);
    glGenBuffers(1, &m_SkyboxVBO);
    glBindVertexArray(m_SkyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SkyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    LoadCubemapTextures(m_CubemapFaces, &m_SkyboxTex);
}

void DrawSkybox(glm::mat4 view, glm::mat4 projection)
{
    glDepthFunc(GL_LEQUAL);
    m_SkyboxShader.Use();
    m_SkyboxShader.SetMat4("view", view);
    m_SkyboxShader.SetMat4("projection", projection);
    m_SkyboxShader.SetFloat("time", glfwGetTime());
    glBindVertexArray(m_SkyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxTex.ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void LoadCubemapTextures(vector<string> faces, Texture* texture)
{
    texture->Path = "";
    texture->Type = "texture_diffuse";
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char* data;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    texture->ID = textureID;
}
