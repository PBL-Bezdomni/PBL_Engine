#pragma once
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glad/glad.h>

class WindowManager
{
private:
	GLFWwindow* m_Window = nullptr;
	bool m_IsInitialized = false;
	
	static void GlfwErrorCallback(int error, const char* description);
	
public:
	const int32_t WINDOW_WIDTH = 2100;
	const int32_t WINDOW_HEIGHT = 1800;
	
	WindowManager();
	GLFWwindow* GetWindowPointer();
	void Initialize(int32_t versionMajor, int32_t versionMinor);
	bool GetIsInitialized();

	void EndFrame();
};
