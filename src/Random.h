#pragma once

class Random
{
private:
	static Random m_Instance;
public:
	static int GetRandomInt(int min, int max);
};
