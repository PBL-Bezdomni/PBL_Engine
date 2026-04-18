#include "WindowManager.h"

#include "Engine.h"

WindowManager::WindowManager()
{}

void WindowManager::Initialize(int32_t versionMajor, int32_t versionMinor)
{
	// Setup window
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
	{
		spdlog::error("Failed to initalize GLFW!");
		m_IsInitialized = false;
		return;
	}

	// GL 4.6 + GLSL 460
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only

	// Create window with graphics context
	m_Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bezdomni", NULL, NULL);
	if (m_Window == NULL)
	{
		spdlog::error("Failed to create GLFW Window!");
		m_IsInitialized = false;
		return;
	}

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1); // Enable VSync - fixes FPS at the refresh rate of your screen

	bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (err)
	{
		spdlog::error("Failed to initialize OpenGL loader!");
		m_IsInitialized = false;
		return;
	}

	// Mouse settings
	// glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glfwSetCursorPosCallback(window, mouse_callback);
	// glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_IsInitialized = true;
}

void WindowManager::GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


GLFWwindow* WindowManager::GetWindowPointer()
{
	return m_Window;
}

bool WindowManager::GetIsInitialized()
{
	return m_IsInitialized;
}

void WindowManager::EndFrame()
{
	glfwPollEvents();
	glfwMakeContextCurrent(m_Window);
	glfwSwapBuffers(m_Window);
}
