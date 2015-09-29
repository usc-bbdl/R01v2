#include "TimeData.h"


TimeData::TimeData(void)
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&initialTick);
}


TimeData::~TimeData(void)
{
}

// Reset the timer
int TimeData::resetTimer(void)
{
	QueryPerformanceCounter(&initialTick);
	return 0;
}


// Get current time in seconds
double TimeData::getCurrentTime(void)
{
	QueryPerformanceCounter(&currentTick);
	actualTime = (double)(currentTick.QuadPart - initialTick.QuadPart);
	actualTime /= (double)frequency.QuadPart;
	return actualTime;
}