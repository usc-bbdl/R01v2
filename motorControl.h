#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <NIDAQmx.h>
#include <utilities.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <ctime>
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
    float64 *windingUpCmnd, *muscleLengthPreviousTick, *muscleLengthOffset, *motorCommand, *loadCellOffset;
    float64 *loadCellData,*motorRef,*muscleLength,*muscleVel, *encoderBias, *encoderGain, *newPdgm_ref, *pcsa, *pennationAngle;
    float *muscleEMG, *spindleIa, *spindleII;
    int expProtocol;
    int *gammaDynamic, *gammaStatic, *cortexDrive;
    double loadCellOffset1, loadCellOffset2, I, tick, tock;
    double fpgaForceGain, fpgaForceOffset;
    HANDLE hIOMutex;
    bool live, expProtocolRunningStateMachine;
    char header[200],dataSample[600];
    bool dataAcquisitionFlag[12], resetMuscleLength;;
    double cortexVoluntaryAmp, cortexVoluntaryFreq;
    TimeData timeData;
    int trialTrigger, No_of_musc;
    double angle, velocity;

    static void motorControlLoop(void*);
    void controlLoop(void);
    int createHeader4DataFile(void);
    int createWindingUpComma(void);
    void createVariables();
    int createWindingUpCommand(void);
    void createDataSampleString(void);
    void setExperimentalProtocol(void);
    void proceedExperimentalProtocol(void);
    void initializeVariables(void);
public:    
    Muscles *muscleObj;
    bool newPdgm_Flag;

    motorControl();
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
    void setForceGainOffset(double forceGain, double forceOffset);
    void setEncoderCalibration(double *encoderGain, double *encoderBias);
    void setMuscleReferenceForce(double *);
    void setMuscleReferenceForce(float *);
    void setMuscleReferenceForceScaling(double *);
    void setMuscleReferenceForceScaling(float *);
    void setMuscleReferenceForceScaling(double,int);
    void setMuscleReferenceForceScaling(float, int);
    void setMuscleReferenceForce(double, int);
    void setMuscleReferenceForce(float, int);
    void getForceGainOffset(double *forceGain, double *forceOffset);
    void getNumberOfMuscles(int *);
    void getLoadCellData(double *);
    void getMuscleLength(double *);
    void getMuscleLength(float *);
    void getMuscleLength(double *, int);
    void getMuscleLength(float *,int );
    void getMuscleVelocity(double *);    
    void getMuscleVelocity(float *);
    void getMuscleVelocity(double *, int);
    void getMuscleVelocity(float *, int);
    //Functions to Communicate with data Logger
    void setMuscleEMG(double*);
    void setMuscleEMG(float*);
    void setSpindleIa(double*);
    void setSpindleIa(float*);
    void setSpindleII(double*);
    void setSpindleII(float*);
    void setSpindleGammaDynamic(int *);
    void setSpindleGammaStatic(int *);
    void setMuscleSpikeCount(int*);
    void setRaster1(int *);
    void setRaster2(int *);
    void setRaster3(int *);
    void setRaster4(int *);
    void setRaster5(int *);
    void setRaster6(int *);
    void setPerturbationAngle(double);
    void setPerturbationVelocity(double);
};
#endif