#pragma once
#include <random>

#undef max
#undef min

class Random
{
public:
	static void InitEngine()
	{
		randomEngine.seed(std::random_device()());
	}

	static float GenFloat()
	{
		return (float)distribution(randomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}

private:
	static std::mt19937 randomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> distribution;
};