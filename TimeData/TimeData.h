#pragma once
#include <windows.h>

class TimeData
{
	//+++ Add status string to send to console
	// or other debugging output

	LARGE_INTEGER initialTick, currentTick, frequency;

public:
	double actualTime;

	// Initialize tick and frequency
	TimeData(void);	

	// Generate status exit string
	~TimeData(void);

	// Resets the timer
	int resetTimer();

	//Get current time in seconds
	double getCurrentTime(void);
};

