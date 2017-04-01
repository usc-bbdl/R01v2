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
    HANDLE hIOMutex;
    bool live;
    float64 *windingUpCmnd, *muscleLengthPreviousTick, *muscleLengthOffset;
    char header[200],dataSample[600];
    bool dataAcquisitionFlag[12];
    TimeData timeData;

    static void motorControlLoop(void*);
    void controlLoop(void);
    int createHeader4DataFile(void);
    int createWindingUpComma(void);
    void motorControl::setExperimentalProtocol(void);
public:    
    Muscles *muscleObj;
    int No_of_musc;
    motorData* mData;
    logger* mLogger;
    bool resetMuscleLength;
    float64 *loadCellData,*motorRef,*muscleLength,*muscleVel, *encoderBias, *encoderGain, *newPdgm_ref;
    float *muscleEMG, *spindleIa, *spindleII;
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    int *muscleSpikeCount, *raster_MN_1, *raster_MN_2, *raster_MN_3, *raster_MN_4, *raster_MN_5, *raster_MN_6;
    int *gammaDynamic, *gammaStatic, *cortexDrive;
    int trialTrigger;
    double angle, velocity;
    bool newPdgm_Flag;

    motorControl(double,double);
    ~motorControl(void);
    bool isEnable, isWindUp, isControlling;
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorControllerStart();
    int motorControllerEnd();
    void setDataAcquisitionFlag(bool *);
    double getTime();
    void dummy();
};

#endif