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
    int *muscleSpikeCount, *raster_MN_1, *raster_MN_2, *raster_MN_3, *raster_MN_4, *raster_MN_5, *raster_MN_6;
    float64 *windingUpCmnd, *muscleLengthPreviousTick, *muscleLengthOffset;
    float64 *loadCellData,*motorRef,*muscleLength,*muscleVel, *encoderBias, *encoderGain, *newPdgm_ref;
    float *muscleEMG, *spindleIa, *spindleII;
    int expProtocol;
    int *gammaDynamic, *gammaStatic, *cortexDrive;
    double loadCellOffset1, loadCellOffset2, I, tick, tock;
    HANDLE hIOMutex;
    bool live, expProtocolRunningStateMachine;
    char header[200],dataSample[600];
    bool dataAcquisitionFlag[12], resetMuscleLength;;
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    TimeData timeData;
    int trialTrigger, No_of_musc;;
    double angle, velocity;

    static void motorControlLoop(void*);
    void controlLoop(void);
    int createHeader4DataFile(void);
    int createWindingUpComma(void);
    void setExperimentalProtocol(void);
    void proceedExperimentalProtocol(void);
public:    
    Muscles *muscleObj;
    motorData* mData;
    logger* mLogger;
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
    void newTrial(int);
    void resetLength();
    //Functions to Communicate with real-time controller
    setEncoderCalibration(double *encoderGain, double *encoderBias);
    setMuscleReferenceForce(double *);
    setMuscleReferenceForce(float *);
    setMuscleReferenceForce(float64 *);
    getLoadCellData(double *);
    getLoadCellData(double *);
    getMuscleVelocity(double *);
    //Functions to Communicate with data Logger
    setMuscleEMG(double*);
    setMuscleEMG(float*);
    setMuscleEMG(float64*);
    setSpindleIa(double*);
    setSpindleIa(float*);
    setSpindleIa(float64*);
    setSpindleII(double*);
    setSpindleII(float*);
    setSpindleII(float64*);
    setSpindleGammaDynamic(int *);
    setSpindleGammaStatic(int *);
    setMuscleSpikeCount(int*);
    setRaster1(int *);
    setRaster2(int *);
    setRaster3(int *);
    setRaster4(int *);
    setRaster5(int *);
    setRaster6(int *);
    setPerturbationAngle(double);
    setPerturbationVelocity(double);
};

#endif