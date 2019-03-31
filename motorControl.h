#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <NIDAQmx.h>
#include <utilities.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <ctime>
#include "matrixFunctions.h"

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
    uInt32      dataEnable;
    TaskHandle  motorTaskHandle, motorEnableHandle,loadCelltaskHandle;
    TaskHandle  encodertaskHandle[3];

    matrixFunctions JR3map;

    double I, perturbationAngle;
    TimeData timeData;
    static void motorControlLoop(void*);
    void controlLoop(void);
    HANDLE hIOMutex;
    bool live;
    float64 encoderData1[1],encoderData2[1],encoderData3[1],muscleLengthPreviousTick[2], muscleLengthOffset[3];
    char header[200];

public:    
    
    double  loadCellOffset[MUSCLE_NUM] , JR3V_offset[NUM_JR3_CHANNELS], JR3F_offset[NUM_JR3_CHANNELS];
    float64 loadCellData[NUM_ANALOG_IN], JR3V       [NUM_JR3_CHANNELS], JR3F       [NUM_JR3_CHANNELS];
    float64 motorRef[MUSCLE_NUM], muscleLength[MUSCLE_NUM], muscleVel[MUSCLE_NUM];
    double CDMRPprotocolTick;

    bool resetMuscleLength;
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    unsigned int muscleSpikeCount[2],raster_MN_1[2],raster_MN_2[2],raster_MN_3[2],raster_MN_4[2],raster_MN_5[2],raster_MN_6[2];
    float muscleEMG[2],spindleIa[2], spindleII[2],encoderBias[3],encoderGain[3];
    motorControl(double *, double *);
    ~motorControl(void);
    bool isEnable, isWindUp, isControlling;
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorControllerStart();
    int motorControllerEnd();
    int gammaDynamic1, gammaStatic1,gammaDynamic2, gammaStatic2/*,trialTrigger*/;
    double cortexDrive[2], angle, velocity;
    bool newPdgm_Flag;
    double newPdgm_ref[7];
    int setPerturbationAngle(double);
    double getTime();
    void dummy();

    void calibrateLC();
};

#endif
