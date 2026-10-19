#pragma once

class Engine;

class Time
{
private:
	static Time m_Instance;
	int m_FPS;
	float m_DeltaTime;
	float m_LastTime;
	
	int m_FrameCounter;
	float m_FrameTimer;

	Engine* m_Engine;
	void FetchEngine();
public:
	Time() = default;
	static void Update();
	static float GetDeltaTime();
	static int GetFPS();
};
