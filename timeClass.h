//#ifndef TIMEDATA_H
//#define TIMEDATA_H
#include <windows.h>

class timeClass
{
    //+++ Add status string to send to console
    // or other debugging output

    //LARGE_INTEGER initialTick, currentTick, frequency;

public:

    double actualTime;

    // Initialize tick and frequency
    timeClass();	

    // Generate status exit string
    ~timeClass();

    // Resets the timer
    int resetTimer();

    //Get current time in seconds
};

//#endif
