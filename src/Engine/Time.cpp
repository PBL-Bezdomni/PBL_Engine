#include "Time.h"
#include <GLFW/glfw3.h>

Time Time::m_Instance;

void Time::Update()
{
	float currentFrame = glfwGetTime();
	m_Instance.m_DeltaTime = currentFrame - m_Instance.m_LastTime;
	m_Instance.m_LastTime = currentFrame;

	m_Instance.m_FrameCounter++;
	m_Instance.m_FrameTimer += m_Instance.m_DeltaTime;
	if (m_Instance.m_FrameTimer >= 1)
	{
		m_Instance.m_FPS = m_Instance.m_FrameCounter;
		m_Instance.m_FrameCounter = 0;
		m_Instance.m_FrameTimer = 0;
	}
}

float Time::GetDeltaTime()
{
	return m_Instance.m_DeltaTime;
}

int Time::GetFPS()
{
	return m_Instance.m_FPS;
}

