#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <NIDAQmx.h>
#include <utilities.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <ctime>
#include "motorData.h"
#include "logger.h"
#include "DAQ.h"
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
    int expProtocol;
    double loadCellOffset1, loadCellOffset2, I, tick, tock;
    TimeData timeData;
    static void motorControlLoop(void*);
    void controlLoop(void);
    HANDLE hIOMutex;
    bool live;
    float64 encoderData1[1],encoderData2[1],muscleLengthPreviousTick[2], muscleLengthOffset[2];
    float64* windingUpCmnd;
    char header[200];
    int createHeader4DataFile(void);
    int createWindingUpComma(void);
    void motorControl::setExperimentalProtocol(void);
    bool dataAcquisitionFlag[12];
    char dataSample[600]="";

public:    
    Muscles *muscleObj;
    int No_of_musc;
    motorData* mData;
    logger* mLogger;

    bool resetMuscleLength;
    float64 *loadCellData,*motorRef,*muscleLength,*muscleVel;
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    unsigned int muscleSpikeCount[2],raster_MN_1[2],raster_MN_2[2],raster_MN_3[2],raster_MN_4[2],raster_MN_5[2],raster_MN_6[2];
    float muscleEMG[2],spindleIa[2], spindleII[2],encoderBias[2],encoderGain[2];
    motorControl(double,double);
    ~motorControl(void);
    bool isEnable, isWindUp, isControlling;
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorControllerStart();
    int motorControllerEnd();
    int* gammaDynamic, gammaStatic, cortexDrive;
    int trialTrigger;
    double angle, velocity;
    bool newPdgm_Flag;
    double newPdgm_ref[2];
    void setDataAcquisitionFlag(bool *);
    double getTime();
    void dummy();
};

#endif