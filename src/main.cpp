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

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

#include "Shader.h"
#include "Model.h"
#include "Entity.h"
#include "Quaternion.h"
#include "Camera.h"
#include "Engine/Loader.h"
#include "Entity.h"
#include "Engine/Engine.h"

#define _USE_MATH_DEFINES

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

void GenerateSkybox();
void DrawSkybox(glm::mat4 view, glm::mat4 projection);

void SetupShaderLight(Shader& shader);

void LoadCubemapTextures(vector<string> faces, Texture* texture);

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
Shader m_SkyboxShader;
Shader m_2DShader;

Texture m_FloorTex;
Texture m_SkyboxTex;
unsigned int m_SkyboxTexID;
Entity world;
Entity skybox;

Entity monkey;
Entity house_floor;
Entity objectsTransform;

unsigned int m_SkyboxVAO;
unsigned int m_SkyboxVBO;

// Values for light
bool m_UseDirLight = true;
bool m_UsePointLight = false;
bool m_UseSpotLight1 = false;
bool m_UseSpotLight2 = false;

glm::vec3 m_PointLightPos = glm::vec3(1.2f, 1.0f, 2.0f);
glm::vec3 m_PointLightColor = glm::vec3(1.f, 1.f, 1.f);
float m_PointLightRadius = 10.f;
float m_PointLightHeight = 5.f;
float m_PointLightLinear = 0.09f;
float m_PointLightQuadratic = 0.032f;

glm::vec3 m_DirLightDirection = glm::vec3(0.f, -1.0f, -1.f);
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

vector<string> m_CubemapFaces { 
    LoadManager.RelativePath + "res/textures/cubemap/right.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/left.jpg",
    LoadManager.RelativePath + "res/textures/cubemap/top.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/bottom.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/front.jpg", 
    LoadManager.RelativePath + "res/textures/cubemap/back.jpg"
};

int main(int, char**)
{
    Engine& engine = Engine::GetInstance();
    engine.Start();
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

    GenerateSkybox();
    m_SkyboxShader.Use();
    m_SkyboxShader.SetInt("skybox", 0);
    
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


        // End frame and swap buffers (double buffering)
        end_frame();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_BasicShader.DeleteProgram();
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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bezdomni", NULL, NULL);
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
    m_BasicShader = Shader((LoadManager.RelativePath + "res/shaders/basic.vert").c_str(), (LoadManager.RelativePath + "res/shaders/basic.frag").c_str());
    m_2DShader = Shader((LoadManager.RelativePath + "res/shaders/basic.vert").c_str(), (LoadManager.RelativePath + "res/shaders/basic.frag").c_str());
    m_SkyboxShader = Shader((LoadManager.RelativePath + "res/shaders/cubemap.vert").c_str(), (LoadManager.RelativePath + "res/shaders/cubemap.frag").c_str());

    LoadTexture((LoadManager.RelativePath + "res/textures/stone.jpg").c_str(), &m_FloorTex);
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
    projection = MainCamera.GetProjectionMatrix(float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);

    //DrawSkybox(skyboxView, projection);

    m_BasicShader.Use();
    m_BasicShader.SetMat4("view", view);
    m_BasicShader.SetMat4("projection", projection);
    SetupShaderLight(m_BasicShader);

    m_2DShader.Use();
    m_2DShader.SetMat4("view", glm::mat4(1.0f));
    m_2DShader.SetMat4("projection", projection);
    SetupShaderLight(m_2DShader);

    world.transform.Position = glm::vec3(0.f, 0.f, -30.f);
    world.transform.Scale = glm::vec3(0.7f);

    house_floor.transform.Scale = glm::vec3(0.1f, 0.1f, 0.1f);
    house_floor.transform.Position = glm::vec3(7.f, 0.f, 0.f);
    house_floor.transform.EulerAngles.x = 90.f;

    monkey.transform.Position = glm::vec3(-5.f, 0.f, 0.f);
    monkey.transform.EulerAngles.y = 45.f;
    monkey.transform.Scale = glm::vec3(2.f);

    m_PointLightPos = quat.RotateQuaternion(glm::vec3(m_PointLightRadius, m_PointLightHeight, 0.f), axisY, glfwGetTime() * 50);

    world.UpdateSelfAndChild();
    world.DrawSelfAndChild();
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
   
}

void imgui_end()
{
    ImGui::Render();
    int display_w, display_h;
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void end_frame()
{
    glfwPollEvents();
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

void LoadModels()
{
    world = Entity();
    objectsTransform = Entity(m_BasicShader);

    house_floor = Entity(m_2DShader, (LoadManager.RelativePath + "res/models/house/floor.obj").c_str());
    house_floor.AssignTexture(m_FloorTex);
    //house_floor.ScaleTexture(FLOOR_TEX_SCALE * FLOOR_SCALE);
    monkey = Entity(m_BasicShader, (LoadManager.RelativePath + "res/models/monkey/Monkey.obj").c_str());
}

void AssignSceneGraph()
{
    world.AddChild(&objectsTransform);

    objectsTransform.AddChild(&house_floor);
    objectsTransform.AddChild(&monkey);

    world.UpdateSelfAndChild();
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
