#pragma once
#include "WinUtil.h"

class Timer
{
public:
	Timer();
	~Timer();

	void Frame();
	float GetTime();

private:
	INT64 frequency;
	INT64 startTime;
	float ticksPerMs;
	float frameTime;
};