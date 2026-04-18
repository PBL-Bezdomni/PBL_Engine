#pragma once

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <spdlog/spdlog.h>
#include "WindowManager.h"

class DebugManager
{
public:
	DebugManager() = default;
	void InitializeImGUI(GLFWwindow* window, const char* glslVersion);
	
	void ImGUIBegin();
	void ImGUIRender();
	void ImGUIEnd();
};
