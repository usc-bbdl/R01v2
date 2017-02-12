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
    TaskHandle  motorTaskHandle, motorEnableHandle,loadCelltaskHandle;
    TaskHandle  encodertaskHandle[2];
    double loadCellOffset1, loadCellOffset2,I;
    TimeData timeData;
    static void motorControlLoop(void*);
    float64 encoderData1[1],encoderData2[1],muscleLengthPreviousTick[2], muscleLengthOffset[2];
    HANDLE hIOMutex;
    bool live;
    //data file header
    char header[200];

    void controlLoop(void);
public:
    bool resetMuscleLength;
    float64 loadCellData[2],motorRef[2],muscleLength[2],muscleVel[2];
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    unsigned int muscleSpikeCount[2],raster_MN_1[2],raster_MN_2[2],raster_MN_3[2],raster_MN_4[2],raster_MN_5[2],raster_MN_6[2];
    float muscleEMG[2],spindleIa[2], spindleII[2],encoderBias[2],encoderGain[2];
    bool isEnable, isWindUp, isControlling;
    int gammaDynamic1, gammaStatic1,gammaDynamic2, gammaStatic2,trialTrigger;
    double cortexDrive[2], angle, velocity;

    motorControl(double,double);
    ~motorControl(void);
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorControllerStart();
    int motorControllerEnd();
    void dummy();
};

#endif
/*

    */