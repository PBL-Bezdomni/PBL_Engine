#include "DebugManager.h"

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
	ImGui::Begin("Camera");
	ImGui::Text("Text");
	ImGui::Button("Button");
	// ImGui::InputFloat("Camera Pos.y", &MainCamera.Position.y);
	// ImGui::InputFloat("Camera Pos.z", &MainCamera.Position.z);
	//
	// ImGui::InputFloat("Camera Yaw", &MainCamera.Yaw);
	// ImGui::InputFloat("Camera Pitch", &MainCamera.Pitch);
	ImGui::End();
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

