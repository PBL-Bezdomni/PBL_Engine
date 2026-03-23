// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

//Check

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
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

//#define IMGUI_IMPL_OPENGL_LOADER_GLAD

//#include <glad/glad.h> // Initialize with gladLoadGL()

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

#include "Shader.h"
#include "Model.h"
#include "Entity.h"
#include "Quat.h"
#include "Camera.h"
#include "Airplane.h"

#define _USE_MATH_DEFINES
#include <math.h>

const float CAMERA_NEAR_PLANE = .1f;
const float CAMERA_FAR_PLANE = 200.f;
const float FLOOR_TEX_SCALE = 8.f;
const float FLOOR_SCALE = 100.f;
const int HOUSE_NET_DIM = 200;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool init();
void init_imgui();
void LoadTexture(const char* path, Texture* texture);
void LoadModels();
void AssignSceneGraph();

void init_shader();

void clear();
void input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void update();
void render();

void imgui_begin();
void imgui_render();
void imgui_end();

void end_frame();

void FillConeMesh(float cx, float cy, float radius, float height);
Entity GenerateCone(Shader& shader);
void UpdateCone(Entity* model);
void GenerateHouseInstanceMatrix();
void GenerateSkybox();
void DrawSkybox(glm::mat4 view, glm::mat4 projection);

void SetDefaultInstanceTransformValues(bool resetValues);
void CheckInstanceTransform();
void SetupShaderLight(Shader& shader);

void LoadCubemapTextures(vector<string> faces, Texture* texture);
void LoadAirplane();
void SetAirplaneGraph();
void UpdateAirplane();

void SwitchFollowAirplane(bool swap = false);
void UpdateCameraFollow();

void SetRefractAirplane();

constexpr int32_t WINDOW_WIDTH = 2100;
constexpr int32_t WINDOW_HEIGHT = 1800;

GLFWwindow* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const char* glsl_version = "#version 410";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 1;

ImVec4 m_ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

Camera MainCamera;

float m_DeltaTime = 0.0f;
float m_LastFrame = 0.0f;

// Mouse fields
float m_MouseLastX = WINDOW_WIDTH / 2; 
float m_MouseLastY = WINDOW_HEIGHT / 2;
bool m_IsFirstMouse = true;

Shader m_BasicShader;
Shader m_InstanceShader;
Shader m_LightSourceShader;
Shader m_SkyboxShader;
Shader m_AirplaneShader;
Shader m_RefractShader;
Shader m_ReflectShader;

Texture m_ConeTex;
Texture m_FloorTex;
Texture m_SkyboxTex;
unsigned int m_SkyboxTexID;
Entity world;
Entity skybox;

Entity refractNanosuit;
Entity reflectNanosuit;

Entity house;
Entity monkey;
Entity house_roof;
Entity house_floor;
Entity objectsTransform;

Entity lightsTransform;
Entity pointLight;
Entity pointLightSphere;
Entity dirLight;
Entity dirLightTransform;
Entity dirLightArrow;
Entity spotLight1;
Entity spotLight1Cone;
Entity spotLight2;
Entity spotLight2Cone;

// Airplane
Airplane airplane;
Entity airplane_body;
Entity l_wing_flap;
Entity r_wing_flap;
Entity l_tail_flap;
Entity r_tail_flap;
Entity m_tail_flap;
Entity propeller;
Entity cockpit;
bool m_FollowAirplane = false;
bool m_LastFollowAirplane = m_FollowAirplane;
bool m_WasFollowRelease = true;

unsigned int m_SkyboxVAO;
unsigned int m_SkyboxVBO;

ImVec4 m_ConeBaseColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
float m_RotationInX = 45.f;
float m_RotationInY = 0.f;
float m_RotationInZ = 0;
float m_ProjectionValue = 45.f;
bool m_RenderWireframe = false;
float m_RefractionIndex = 1.52f;
float m_RRefIndex = 2.1f;
float m_GRefIndex = 1.12f;
float m_BRefIndex = 2.42f;
bool m_UseChromaticRefraction = false;

// Values for instances
bool m_MoveOnlyRoofInstance = false;
bool m_LastMoveRoofFlagState;
int m_InstanceIndex = 20100;
int m_LastInstanceIndex;
float m_CurrInstanceTransformX;
float m_CurrInstanceTransformY;
float m_CurrInstanceTransformZ;
glm::vec3 m_LastInstanceTransform;
float m_CurrInstanceRotationX;
float m_CurrInstanceRotationY;
float m_CurrInstanceRotationZ;
glm::vec3 m_LastInstanceRotation;
float m_CurrInstanceScaleX;
float m_CurrInstanceScaleY;
float m_CurrInstanceScaleZ;
glm::vec3 m_LastInstanceScale;

int m_ConeSteps = 20;
int m_LastConeSteps = m_ConeSteps;
float m_ConeCenterX = 0;
float m_ConeCenterY = 0;
float m_ConeRadius = 1;
float m_ConeHeight = 2;

// Values for light
bool m_UseDirLight = true;
bool m_UsePointLight = false;
bool m_UseSpotLight1 = false;
bool m_UseSpotLight2 = false;

bool m_RenderNanoSuits = true;

bool m_RefractAirplane = true;
bool m_LastRefractAirplane = m_RefractAirplane;

glm::vec3 m_PointLightPos = glm::vec3(1.2f, 1.0f, 2.0f);
glm::vec3 m_PointLightColor = glm::vec3(1.f, 1.f, 1.f);
float m_PointLightRadius = 10.f;
float m_PointLightHeight = 5.f;
float m_PointLightLinear = 0.09f;
float m_PointLightQuadratic = 0.032f;

glm::vec3 m_DirLightDirection = glm::vec3(0.2f, -1.0f, -3.3f);
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

int houseNetSize = HOUSE_NET_DIM * HOUSE_NET_DIM;
vector<glm::mat4> HouseInstanceMatrix;
vector<glm::mat4> RoofInstanceMatrix;

vector<Vertex> m_ConeVertices;
vector<unsigned int> m_ConeIndices;
vector<Texture>      m_ConeTextures;

const string relativePath = "../../";
//const string relativePath = "../../../";

vector<string> m_CubemapFaces { 
    relativePath + "res/textures/cubemap/right.jpg", 
    relativePath + "res/textures/cubemap/left.jpg",
    relativePath + "res/textures/cubemap/top.jpg", 
    relativePath + "res/textures/cubemap/bottom.jpg", 
    relativePath + "res/textures/cubemap/front.jpg", 
    relativePath + "res/textures/cubemap/back.jpg"
};

int main(int, char**)
{
    if (!init())
    {
        spdlog::error("Failed to initialize project!");
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized project.");

    init_imgui();
    spdlog::info("Initialized ImGui.");

    init_shader();

    MainCamera = Camera(glm::vec3(0.f, 0.f, 3.f));

    LoadModels();
    AssignSceneGraph();

    glEnable(GL_DEPTH_TEST);

    // Default Instance Settings
    SetDefaultInstanceTransformValues(true);
    //m_SkyboxShader.use();
    GenerateSkybox();
    m_SkyboxShader.use();
    m_SkyboxShader.setInt("skybox", 0);

    pointLight.SetModelRenderBool(m_UsePointLight);
    spotLight1.SetModelRenderBool(m_UseSpotLight1);
    spotLight2.SetModelRenderBool(m_UseSpotLight2);
    
    //m_SkyboxShader.setInt("skybox", 0);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear screen
        clear();

        // Process I/O operations here
        input(window);

        // Update game objects' state here
        update();

        // OpenGL rendering code here
        render();

        // Draw ImGui
        imgui_begin();
        imgui_render(); // edit this function to add your own ImGui controls
        imgui_end();    // this call effectively renders ImGui

        // Check instance transform
        CheckInstanceTransform();

        // End frame and swap buffers (double buffering)
        end_frame();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_BasicShader.deleteProgram();
    m_InstanceShader.deleteProgram();
    m_LightSourceShader.deleteProgram();
    //glDeleteProgram(m_ShaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

bool init()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        spdlog::error("Failed to initalize GLFW!");
        return false;
    }

    // GL 4.6 + GLSL 460
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only

    // Create window with graphics context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "House Village Piotr Gortynski 216763", NULL, NULL);
    if (window == NULL)
    {
        spdlog::error("Failed to create GLFW Window!");
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync - fixes FPS at the refresh rate of your screen

    bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (err)
    {
        spdlog::error("Failed to initialize OpenGL loader!");
        return false;
    }

    // Mouse settings
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    /*FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }*/

    return true;
}

void init_imgui()
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
}

void init_shader()
{
    m_BasicShader = Shader((relativePath + "res/shaders/basic.vert").c_str(), (relativePath + "res/shaders/basic.frag").c_str());
    m_AirplaneShader = Shader((relativePath + "res/shaders/basic.vert").c_str(), (relativePath + "res/shaders/basic.frag").c_str());
    m_InstanceShader = Shader((relativePath + "res/shaders/instance.vert").c_str(), (relativePath + "res/shaders/lighting.frag").c_str());
    m_LightSourceShader = Shader((relativePath + "res/shaders/lightsource.vert").c_str(), (relativePath + "res/shaders/lightsource.frag").c_str());
    m_SkyboxShader = Shader((relativePath + "res/shaders/cubemap.vert").c_str(), (relativePath + "res/shaders/cubemap.frag").c_str());
    m_ReflectShader = Shader((relativePath + "res/shaders/reflection.vert").c_str(), (relativePath + "res/shaders/reflection.frag").c_str());
    m_RefractShader = Shader((relativePath + "res/shaders/refraction.vert").c_str(), (relativePath + "res/shaders/refraction.frag").c_str());

    LoadTexture((relativePath + "res/textures/stone.jpg").c_str(), &m_FloorTex);
}

void clear()
{
    // Clear z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Clear space with full color
    glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (m_WasFollowRelease)
        {
            SwitchFollowAirplane(true);
            m_WasFollowRelease = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
    {
        m_WasFollowRelease = true;
    }
    if (!m_FollowAirplane)
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
    else
    {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            airplane.UpdateYaw(-1);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            airplane.UpdateYaw(1);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
        {
            airplane.UpdateYaw(0);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            airplane.UpdatePitch(1);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            airplane.UpdatePitch(-1);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
        {
            airplane.UpdatePitch(0);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            airplane.UpdateSpeed(1);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            airplane.UpdateSpeed(-1);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
        {
            airplane.UpdateSpeed(0);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && !m_FollowAirplane)
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

    if (m_FollowAirplane != m_LastFollowAirplane)
    {
        SwitchFollowAirplane();
    }
    if (m_FollowAirplane)
    {
        UpdateCameraFollow();
    }

    if (m_LastRefractAirplane != m_RefractAirplane)
    {
        SetRefractAirplane();
    }
}

void render()
{
    glm::vec3 axisX = glm::vec3(1.0f, 0.f, 0.f);
    glm::vec3 axisY = glm::vec3(.0f, 1.f, 0.f);
    glm::vec3 axisZ = glm::vec3(.0f, 0.f, 1.f);
    Quat quat = Quat();
    
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(MainCamera.GetViewMatrix()));;
    // note that we're translating the scene in the reverse direction of where we want to move
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = MainCamera.GetViewMatrix();
   
    glm::mat4 projection;
    projection = MainCamera.GetProjectionMatrix(float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);

    //DrawSkybox(skyboxView, projection);

    glm::vec3 dirrotTemp = quat.rotate_quat(glm::vec3(0.f, -1.f, 0.f), axisX, m_DirLightAngleX);
    m_DirLightDirection = quat.rotate_quat(dirrotTemp, axisY, m_DirLightAngleZ);
    m_SpotLight1Direction = quat.rotate_quat(glm::vec3(0.f, 0.f, 1.f), axisY, m_SpotLight1Angle);
    m_SpotLight2Direction = quat.rotate_quat(glm::vec3(0.f, 0.f, 1.f), axisY, m_SpotLight2Angle);

    m_BasicShader.use();
    m_BasicShader.setMat4("view", view);
    m_BasicShader.setMat4("projection", projection);
    SetupShaderLight(m_BasicShader);

    m_InstanceShader.use();
    m_InstanceShader.setMat4("view", view);
    m_InstanceShader.setMat4("projection", projection);
    SetupShaderLight(m_InstanceShader);

    m_AirplaneShader.use();
    m_AirplaneShader.setMat4("view", view);
    m_AirplaneShader.setMat4("projection", projection);
    SetupShaderLight(m_AirplaneShader);

    m_LightSourceShader.use();
    m_LightSourceShader.setMat4("view", view);
    m_LightSourceShader.setMat4("projection", projection);

    m_ReflectShader.use();
    m_ReflectShader.setMat4("view", view);
    m_ReflectShader.setMat4("projection", projection);
    m_ReflectShader.setVec3("cameraPos", MainCamera.Position);
    m_ReflectShader.setInt("skybox", 0);

    m_RefractShader.use();
    m_RefractShader.setMat4("view", view);
    m_RefractShader.setMat4("projection", projection);
    m_RefractShader.setVec3("cameraPos", MainCamera.Position);
    m_RefractShader.setInt("skybox", 0);
    m_RefractShader.setFloat("refraction_index", m_RefractionIndex);
    glm::vec3 chromaticVector = glm::vec3(m_RRefIndex, m_GRefIndex, m_BRefIndex);
    m_RefractShader.setVec3("rgb_refraction_index", chromaticVector);
    m_RefractShader.setBool("use_chromatic_refraction", m_UseChromaticRefraction);

    world.transform.pos = glm::vec3(0.f, 0.f, -30.f);
    world.transform.scale = glm::vec3(0.7f);

    house_floor.transform.scale = glm::vec3(0.1f, 0.1f, 0.1f);
    house_floor.transform.pos = glm::vec3(5.f, 0.f, 0.f);
    house_floor.transform.eulerRot.x = 45.f;

    monkey.transform.pos = glm::vec3(-5.f, 0.f, 0.f);
    monkey.transform.scale = glm::vec3(2.f);

    m_PointLightPos = quat.rotate_quat(glm::vec3(m_PointLightRadius, m_PointLightHeight, 0.f), axisY, glfwGetTime() * 50);
    pointLight.transform.pos = m_PointLightPos;
    pointLight.transform.scale = glm::vec3(0.2f);

    dirLight.transform.pos = m_DirLightVisualPos;
    dirLight.transform.eulerRot.y = m_DirLightAngleZ;
    dirLightArrow.transform.eulerRot.x = -90.f;
    dirLightTransform.transform.eulerRot.x = m_DirLightAngleX;

    spotLight1.transform.pos = m_SpotLight1Position;
    spotLight1.transform.eulerRot.y = m_SpotLight1Angle;
    spotLight2.transform.pos = m_SpotLight2Position;
    spotLight2.transform.eulerRot.y = m_SpotLight2Angle;

    refractNanosuit.transform.scale = glm::vec3(.5f);
    refractNanosuit.transform.pos.x = 20.f;
    reflectNanosuit.transform.scale = glm::vec3(.5f);
    reflectNanosuit.transform.pos.x = -20.f;

    UpdateAirplane();

    reflectNanosuit.SetModelRenderBool(m_RenderNanoSuits);
    refractNanosuit.SetModelRenderBool(m_RenderNanoSuits);

    world.updateSelfAndChild();
    world.drawSelfAndChild();
}

void LoadTexture(const char* path, Texture* tex)
{
    tex->path = path;
    tex->type = "texture_diffuse";
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);
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
        spdlog::info("Loaded Texture");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        spdlog::error("Failed to load texture");
        spdlog::error(stbi_failure_reason());
    }
    stbi_image_free(data);
}

void imgui_begin()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_render()
{
    ImGui::Begin("Refraction settings");
    ImGui::Checkbox("Follow airplane", &m_FollowAirplane);
    ImGui::Checkbox("Render suits", &m_RenderNanoSuits);
    ImGui::Checkbox("Set airplane to refract", &m_RefractAirplane);
    ImGui::Checkbox("Use color refraction", &m_UseChromaticRefraction);
    if (!m_UseChromaticRefraction)
    {
        ImGui::SliderFloat("Refraction value", &m_RefractionIndex, 1.f, 2.42f);
    }
    else
    {
        ImGui::SliderFloat("Red refraction value", &m_RRefIndex, 1.f, 2.42f);
        ImGui::SliderFloat("Green refraction value", &m_GRefIndex, 1.f, 2.42f);
        ImGui::SliderFloat("Blue refraction value", &m_BRefIndex, 1.f, 2.42f);
    }
    ImGui::End();
}

void imgui_end()
{
    ImGui::Render();
    int display_w, display_h;
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    // glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);
    // glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void end_frame()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

Entity GenerateCone(Shader& shader)
{
    FillConeMesh(m_ConeCenterX, m_ConeCenterY, m_ConeRadius, m_ConeHeight);
    return Entity(shader, m_ConeVertices, m_ConeIndices, m_ConeTextures);
}

void UpdateCone(Entity* model)
{
    FillConeMesh(m_ConeCenterX, m_ConeCenterY, m_ConeRadius, m_ConeHeight);
    model->UpdateVertices(m_ConeVertices, m_ConeIndices, m_ConeTextures);
}

void FillConeMesh(float cx, float cy, float radius, float height)
{
    m_ConeVertices.clear();
    m_ConeIndices.clear();
    m_ConeTextures.clear();

    Vertex tip;
    glm::vec3 pos = glm::vec3(cx, cy, -height);
    tip.Position = pos;
    glm::vec3 normal = glm::vec3(m_ConeBaseColor.x, m_ConeBaseColor.y, m_ConeBaseColor.z);
    tip.Normal = normal;
    glm::vec2 texCoords = glm::vec2(1.0, 1.0);
    tip.TexCoords = texCoords;

    Vertex base;
    pos = glm::vec3(cx, cy, 0);
    base.Position = pos;
    base.Normal = normal;
    texCoords = glm::vec2(0.0, 1.0);
    base.TexCoords = texCoords;

    m_ConeVertices.push_back(tip);
    m_ConeVertices.push_back(base);

    Vertex v1;
    pos = glm::vec3(cx + radius, cy, 0);
    v1.Position = pos;
    v1.Normal = normal;
    glm::vec2 texCOdd = glm::vec2(0.0, 0.0);
    v1.TexCoords = texCOdd;

    m_ConeVertices.push_back(v1);

    Vertex v2;
    v2.Normal = normal;
    glm::vec2 texCEven = glm::vec2(1.0, 0.0);

    for (int i = 1; i < m_ConeSteps; i++)
    {
        float theta = 2.0f * M_PI * (float(i) / float(m_ConeSteps)); // get the current angle

        float x = radius * cosf(theta); // calculate the x component
        float y = radius * sinf(theta); // calculate the y component

        pos.x = x + cx; // output vertex
        pos.y = y + cy; // output vertex
        v2.Position = pos;

        if (i % 2 != 0)
        {
            v2.TexCoords = texCEven;
        }
        else
        {
            v2.TexCoords = texCOdd;
        }

        m_ConeVertices.push_back(v2);

        m_ConeIndices.push_back(0);
        m_ConeIndices.push_back(1 + i);
        m_ConeIndices.push_back(2 + i);

        m_ConeIndices.push_back(1);
        m_ConeIndices.push_back(1 + i);
        m_ConeIndices.push_back(2 + i);
    }

    m_ConeIndices.push_back(0);
    m_ConeIndices.push_back(m_ConeSteps + 1);
    m_ConeIndices.push_back(2);

    m_ConeIndices.push_back(1);
    m_ConeIndices.push_back(m_ConeSteps + 1);
    m_ConeIndices.push_back(2);

    m_ConeTextures.push_back(m_ConeTex);
}

void LoadModels()
{
    GenerateHouseInstanceMatrix();

    world = Entity();
    objectsTransform = Entity(m_BasicShader);

    house = Entity(m_InstanceShader, (relativePath + "res/models/house/house.obj").c_str(), houseNetSize, HouseInstanceMatrix);
    house_roof = Entity(m_InstanceShader, (relativePath + "res/models/house/roof.obj").c_str(), houseNetSize, RoofInstanceMatrix);
    house_floor = Entity(m_BasicShader, (relativePath + "res/models/house/floor.obj").c_str());
    house_floor.AssignTexture(m_FloorTex);
    //house_floor.ScaleTexture(FLOOR_TEX_SCALE * FLOOR_SCALE);
    monkey = Entity(m_BasicShader, (relativePath + "res/models/monkey/Monkey.obj").c_str());

    lightsTransform = Entity(m_LightSourceShader);
    pointLight = Entity(m_LightSourceShader);
    pointLightSphere = Entity(m_LightSourceShader, (relativePath + "res/models/sphere/sphere.obj").c_str());
    dirLight = Entity(m_LightSourceShader);
    dirLightTransform = Entity(m_LightSourceShader);
    dirLightArrow = GenerateCone(m_LightSourceShader);
    spotLight1 = Entity(m_LightSourceShader);
    spotLight1Cone = GenerateCone(m_LightSourceShader);
    spotLight2 = Entity(m_LightSourceShader);
    spotLight2Cone = GenerateCone(m_LightSourceShader);

    refractNanosuit = Entity(m_RefractShader, (relativePath + "res/models/nanosuit/nanosuit.obj").c_str());
    reflectNanosuit = Entity(m_ReflectShader, (relativePath + "res/models/nanosuit/nanosuit.obj").c_str());

    LoadAirplane();
}

void AssignSceneGraph()
{
    world.addChild(&objectsTransform);
    //world.addChild(&lightsTransform);

    objectsTransform.addChild(&house_floor);
    objectsTransform.addChild(&monkey);
    //objectsTransform.addChild(&house);
    //objectsTransform.addChild(&airplane);
    //objectsTransform.addChild(&refractNanosuit);
    //objectsTransform.addChild(&reflectNanosuit);

    //house.addChild(&house_roof);

    //lightsTransform.addChild(&pointLight);
    //lightsTransform.addChild(&dirLight);
    //lightsTransform.addChild(&spotLight1);
    //lightsTransform.addChild(&spotLight2);

    //pointLight.addChild(&pointLightSphere);
    //dirLight.addChild(&dirLightTransform);
    //dirLightTransform.addChild(&dirLightArrow);
    //spotLight1.addChild(&spotLight1Cone);
    //spotLight2.addChild(&spotLight2Cone);

    world.updateSelfAndChild();
}

void GenerateHouseInstanceMatrix()
{
    unsigned int amount = houseNetSize;
    float offset = 10.f;
    float startCoord = -((float)HOUSE_NET_DIM / 2.f) * offset;

    for (unsigned x = 0; x < HOUSE_NET_DIM; x++)
    {
        for (unsigned int z = 0; z < HOUSE_NET_DIM; z++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(startCoord + (x * offset), 0.f, startCoord + (z * offset)));
            HouseInstanceMatrix.push_back(model);
            RoofInstanceMatrix.push_back(model);
        }
    }
}

void SetDefaultInstanceTransformValues(bool resetValues)
{
    m_LastInstanceIndex = m_InstanceIndex;
    m_LastMoveRoofFlagState = m_MoveOnlyRoofInstance;

    if (resetValues)
    {
        m_CurrInstanceTransformX = 0;
        m_CurrInstanceTransformY = 0;
        m_CurrInstanceTransformZ = 0;

        m_CurrInstanceScaleX = 1;
        m_CurrInstanceScaleY = 1;
        m_CurrInstanceScaleZ = 1;

        m_CurrInstanceRotationX = 0;
        m_CurrInstanceRotationY = 0;
        m_CurrInstanceRotationZ = 0;
    }

    m_LastInstanceTransform = glm::vec3(m_CurrInstanceTransformX, m_CurrInstanceTransformY, m_CurrInstanceTransformZ);
    m_LastInstanceScale = glm::vec3(m_CurrInstanceScaleX, m_CurrInstanceScaleY, m_CurrInstanceScaleZ);
    m_LastInstanceRotation = glm::vec3(m_CurrInstanceRotationX, m_CurrInstanceRotationY, m_CurrInstanceRotationZ);
}

void CheckInstanceTransform()
{
    glm::vec3 currInstTransform = glm::vec3(m_CurrInstanceTransformX, m_CurrInstanceTransformY, m_CurrInstanceTransformZ);
    glm::vec3 currInstScale = glm::vec3(m_CurrInstanceScaleX, m_CurrInstanceScaleY, m_CurrInstanceScaleZ);
    glm::vec3 currInstRotation = glm::vec3(m_CurrInstanceRotationX, m_CurrInstanceRotationY, m_CurrInstanceRotationZ);

    if (m_LastMoveRoofFlagState != m_MoveOnlyRoofInstance || m_LastInstanceIndex != m_InstanceIndex)
    {
        if (m_LastMoveRoofFlagState)
        {
            house_roof.updateSelfAndChildInstanceMatrix(currInstTransform, currInstRotation, currInstScale, m_LastInstanceIndex, m_InstanceShader, true);
        }
        else
        {
            house.updateSelfAndChildInstanceMatrix(currInstTransform, currInstRotation, currInstScale, m_LastInstanceIndex, m_InstanceShader, true);
        }
        SetDefaultInstanceTransformValues(true);
    }
    else if (currInstTransform != m_LastInstanceTransform || currInstScale != m_LastInstanceScale || currInstRotation != m_LastInstanceRotation)
    {
        if (m_MoveOnlyRoofInstance)
        {
            house_roof.updateSelfAndChildInstanceMatrix(currInstTransform, currInstRotation, currInstScale, m_InstanceIndex, m_InstanceShader, false);
        }
        else
        {
            house.updateSelfAndChildInstanceMatrix(currInstTransform, currInstRotation, currInstScale, m_InstanceIndex, m_InstanceShader, false);
        }
        SetDefaultInstanceTransformValues(false);
    }
}

void SetupShaderLight(Shader& shader)
{
    shader.setBool("useDirLight", m_UseDirLight);
    shader.setVec3("dirLight.color", m_DirLightColor);
    shader.setVec3("dirLight.direction", m_DirLightDirection);

    shader.setBool("usePointLight", m_UsePointLight);
    shader.setVec3("pointLight.color", m_PointLightColor);
    shader.setVec3("pointLight.position", m_PointLightPos);
    shader.setFloat("pointLight.constant", 1.0f);
    shader.setFloat("pointLight.linear", m_PointLightLinear);
    shader.setFloat("pointLight.quadratic", m_PointLightQuadratic);
    shader.setVec3("viewPos", MainCamera.Position);

    shader.setBool("useSpotLight1", m_UseSpotLight1);
    shader.setVec3("spotLight1.color", m_SpotLight1Color);
    shader.setVec3("spotLight1.position", m_SpotLight1Position);
    shader.setVec3("spotLight1.direction", m_SpotLight1Direction);
    shader.setFloat("spotLight1.cutOff", glm::cos(glm::radians(m_SpotLight1CutOff)));
    shader.setFloat("spotLight1.outerCutOff", glm::cos(glm::radians(m_SpotLight1OuterCutOff)));
    shader.setFloat("spotLight1.constant", 1.0f);
    shader.setFloat("spotLight1.linear", m_SpotLight1Linear);
    shader.setFloat("spotLight1.quadratic", m_SpotLight2Quadratic);

    shader.setBool("useSpotLight2", m_UseSpotLight2);
    shader.setVec3("spotLight2.color", m_SpotLight2Color);
    shader.setVec3("spotLight2.position", m_SpotLight2Position);
    shader.setVec3("spotLight2.direction", m_SpotLight2Direction);
    shader.setFloat("spotLight2.cutOff", glm::cos(glm::radians(m_SpotLight2CutOff)));
    shader.setFloat("spotLight2.outerCutOff", glm::cos(glm::radians(m_SpotLight2OuterCutOff)));
    shader.setFloat("spotLight2.constant", 1.0f);
    shader.setFloat("spotLight2.linear", m_SpotLight2Linear);
    shader.setFloat("spotLight2.quadratic", m_SpotLight2Quadratic);
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

    m_SkyboxShader.use();
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
    m_SkyboxShader.use();
    m_SkyboxShader.setMat4("view", view);
    m_SkyboxShader.setMat4("projection", projection);
    glBindVertexArray(m_SkyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxTex.id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void LoadCubemapTextures(vector<string> faces, Texture* texture)
{
    texture->path = "";
    texture->type = "texture_diffuse";
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

    texture->id = textureID;
}

void SetAirplaneGraph()
{
    airplane.addChild(&airplane_body);

    airplane_body.addChild(&propeller);
    airplane_body.addChild(&cockpit);

    airplane_body.addChild(&l_wing_flap);
    airplane_body.addChild(&r_wing_flap);

    airplane_body.addChild(&l_tail_flap);
    airplane_body.addChild(&r_tail_flap);
    airplane_body.addChild(&m_tail_flap);
}

void LoadAirplane()
{
    airplane_body = Entity(m_RefractShader, (relativePath + "res/models/airplane/airplane_body.obj").c_str());

    propeller = Entity(m_AirplaneShader, (relativePath + "res/models/airplane/propeller.obj").c_str());
    cockpit = Entity(m_ReflectShader, (relativePath + "res/models/airplane/cockpit.obj").c_str());

    l_wing_flap = Entity(m_AirplaneShader, (relativePath + "res/models/airplane/l_wing_flap.obj").c_str());
    r_wing_flap = Entity(m_AirplaneShader, (relativePath + "res/models/airplane/r_wing_flap.obj").c_str());

    l_tail_flap = Entity(m_AirplaneShader, (relativePath + "res/models/airplane/l_tail_flap.obj").c_str());
    r_tail_flap = Entity(m_AirplaneShader, (relativePath + "res/models/airplane/r_tail_flap.obj").c_str());
    m_tail_flap = Entity(m_AirplaneShader, (relativePath + "res/models/airplane/m_tail_flap.obj").c_str());

    airplane = Airplane(m_AirplaneShader);
    SetAirplaneGraph();
    airplane.SetDefault();
}

void UpdateAirplane()
{
    airplane.Update(m_DeltaTime);
    airplane.transform.eulerRot.y = -airplane.Yaw - 90;
    airplane.transform.eulerRot.z = -airplane.Roll;
    airplane_body.transform.eulerRot.x = airplane.Pitch;
    propeller.transform.eulerRot.z += m_DeltaTime * airplane.Speed * airplane.PropellerSpeedMult;
    float velocity = m_DeltaTime * airplane.Speed;
    airplane.transform.pos += velocity * airplane.Front;

    l_tail_flap.transform.pos.y = airplane.PitchOffset * airplane.maxFlapTilt;
    r_tail_flap.transform.pos.y = airplane.PitchOffset * airplane.maxFlapTilt;

    l_wing_flap.transform.pos.y = -airplane.YawOffset * airplane.maxFlapTilt;
    r_wing_flap.transform.pos.y = airplane.YawOffset * airplane.maxFlapTilt;
    m_tail_flap.transform.pos.x = airplane.YawOffset * airplane.maxFlapTilt;
}

void SwitchFollowAirplane(bool swap)
{
    if (swap)
    {
        m_FollowAirplane = !m_FollowAirplane;
    }
    m_LastFollowAirplane = m_FollowAirplane;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    airplane.SetRemoteControl(m_FollowAirplane);
}

void UpdateCameraFollow()
{
    if (m_FollowAirplane)
    {
        MainCamera.Position = airplane.GetWorldPosition() - (airplane.Front * 25.f);
        MainCamera.Position.y += 1.f;
        MainCamera.Yaw = airplane.Yaw - (airplane.YawIncrease / 4.f);
        MainCamera.Pitch = airplane.Pitch - (airplane.Pitch / 4);
        MainCamera.UpdateCameraVectors();
    }
}

void SetRefractAirplane()
{
    m_LastRefractAirplane = m_RefractAirplane;
    if (m_RefractAirplane)
    {
        airplane_body.ReassignShader(m_RefractShader);
    }
    else
    {
        airplane_body.ReassignShader(m_AirplaneShader);
    }
}