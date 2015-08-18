#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <NIDAQmx.h>
#include <utilities.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <ctime>

class TimeData
{
      LARGE_INTEGER initialTick, currentTick, frequency;
public:

    double actualTime;
    TimeData(void);	
    ~TimeData(void);
    int resetTimer();
    double getCurrentTime(void);
};

class motorControl
{
    TaskHandle  motorTaskHandle, motorEnableHandle,loadCelltaskHandle,analogClient;
    TaskHandle  encodertaskHandle[2];
    double loadCellOffset1, loadCellOffset2,I;
    TimeData timeData;
    static void motorControlLoop(void*);
    void controlLoop(void);
    HANDLE hIOMutex;
    bool live;
    float64 encoderData1[1],encoderData2[1],muscleLengthPreviousTick[2], muscleLengthOffset[2];
public:
    bool resetMuscleLength;
    float64 loadCellData[2],motorRef[2],muscleLength[2],muscleVel[2];
    motorControl(double,double);
    ~motorControl(void);
    bool isEnable, isWindUp, isControlling;
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorControllerStart();
    int motorControllerEnd();
};

#endif

