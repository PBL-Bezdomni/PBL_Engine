#include <random>
#include <cmath>
#include "Random.h"

Random Random::m_Instance;

int Random::GetRandomInt(int min, int max)
{
	max = abs(min) + abs(max);
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0,max);
	int r = dist(rng);
	r -= abs(min);
	return r;
}
