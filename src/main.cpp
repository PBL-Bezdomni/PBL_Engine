#include <GLFW/glfw3.h>
#include "Engine/Engine.h"
#include "Engine/DebugManager.h"

WindowManager* WindowMgr = nullptr;

int main(int, char**)
{
    Engine& engine = Engine::GetInstance();

    WindowMgr = &engine.GetWindowManager();
    
    engine.Start();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(WindowMgr->GetWindowPointer());
    glfwTerminate();

    return 0;
}
