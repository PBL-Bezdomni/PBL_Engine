#pragma once

class Random
{
private:
	static Random m_Instance;
public:
	static int GetRandomInt(int min, int max);
	static float GetRandomFloat(float min, float max);
	static void Shuffle(std::vector<int>& vec);
};
