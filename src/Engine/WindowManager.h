#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class WindowManager
{
private:
	const int32_t WINDOW_WIDTH = 2100;
	const int32_t WINDOW_HEIGHT = 1800;
	
	GLFWwindow* m_Window = nullptr;
	const GLFWvidmode* m_VideoMode = nullptr;
	GLFWmonitor* m_Monitor = nullptr;
	bool m_IsInitialized = false;
	glm::vec4 m_ClearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);

	int32_t m_Width;
	int32_t m_Height;
	
	static void GlfwErrorCallback(int error, const char* description);

	bool m_IsFullWindow = false;
	
public:
	float GetWindowWidth();
	float GetWindowHeight();
	float GetWindowAspectRatio();
	
	WindowManager() = default;
	GLFWwindow* GetWindowPointer();
	void Initialize(int32_t versionMajor, int32_t versionMinor);
	bool GetIsInitialized();

	void ClearFrame();
	void EndFrame();

	bool ShouldWindowClose();
};
