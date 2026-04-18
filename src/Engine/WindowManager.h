#pragma once
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

class WindowManager
{
private:
	GLFWwindow* m_Window = nullptr;
	bool m_IsInitialized = false;
	glm::vec4 m_ClearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	static void GlfwErrorCallback(int error, const char* description);
	
public:
	const int32_t WINDOW_WIDTH = 2100;
	const int32_t WINDOW_HEIGHT = 1800;
	
	WindowManager() = default;
	GLFWwindow* GetWindowPointer();
	void Initialize(int32_t versionMajor, int32_t versionMinor);
	bool GetIsInitialized();

	void ClearFrame();
	void EndFrame();
};
