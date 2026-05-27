#include <random>
#include <cmath>
#include "Random.h"

Random Random::m_Instance;

int Random::GetRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 rng(rd());

	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

float Random::GetRandomFloat(float min, float max)
{
	std::random_device rd;
	std::mt19937 rng(rd());

	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

void Random::Shuffle(std::vector<int>& vec)
{
	std::random_device rd;
	std::mt19937 rng(rd());

	std::shuffle(vec.begin(), vec.end(), rng);
}
