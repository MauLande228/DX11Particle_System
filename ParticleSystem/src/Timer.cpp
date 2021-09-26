#include "Timer.h"

Timer::Timer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

	ticksPerMs = (float)(frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
}

Timer::~Timer()
{
}

void Timer::Frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	timeDifference = (float)(currentTime - startTime);
	frameTime = timeDifference / ticksPerMs;
	startTime = currentTime;
}

float Timer::GetTime()
{
	return frameTime;
}
